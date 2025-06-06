//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#elif __linux__
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <array>
#endif
#include <iostream>
#include <filesystem>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//hub
#include "gui.hpp"
#include "render.hpp"
#include "core.hpp"
#include "createproject.hpp"
#include "removeproject.hpp"

using Core::Hub;
using Graphics::Render;

#ifdef _WIN32
using std::wstring;
#elif __linux__
using std::array;
#endif
using std::cout;
using std::getline;
using std::filesystem::is_empty;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using std::filesystem::remove_all;
using std::filesystem::remove;
using std::filesystem::rename;
using std::filesystem::copy;
using std::filesystem::create_directory;
using std::to_string;
using std::filesystem::is_directory;

namespace Graphics::GUI
{
	void GUI_Hub::Initialize()
	{
		cout << "Initializing ImGui.\n\n";

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		static string tempString = (path(Hub::docsPath) / "imgui.ini").string();
		const char* customConfigPath = tempString.c_str();
		io.IniFilename = customConfigPath;

		ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		string fontPath = (path(Hub::defaultPath) / "files" / "fonts" / "coda" / "Coda-Regular.ttf").string();
		if (exists(fontPath))
		{
			io.Fonts->Clear();
			io.Fonts->AddFontFromFileTTF(((path(Hub::defaultPath) / "files" / "fonts" / "coda" / "Coda-Regular.ttf")).string().c_str(), 16.0f);
		}
		else
		{
			cout << "Error: Font " << fontPath << " does not exist!\n\n";
		}

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		style.TabRounding = 6;
		style.FramePadding = ImVec2(6, 2);
		style.ItemSpacing = ImVec2(0, 5);
		io.FontGlobalScale = 1.2f;

		isImguiInitialized = true;
	}

	ImVec2 GUI_Hub::CenterWindow(const ImVec2& size)
	{
		int intWidth, intHeight;
		glfwGetFramebufferSize(Render::window, &intWidth, &intHeight);

		float glfwWindowWidth = static_cast<float>(intWidth);
		float glfwWindowHeight = static_cast<float>(intHeight);

		float posX = (glfwWindowWidth - size.x) / 2.0f;
		float posY = (glfwWindowHeight - size.y) / 2.0f;

		return ImVec2(posX, posY);
	}

	void GUI_Hub::Render()
	{
		if (isImguiInitialized)
		{
			if (!Hub::IsUserIdle())
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				ImGuiDockNodeFlags dockFlags =
					ImGuiDockNodeFlags_PassthruCentralNode;

				ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockFlags, nullptr);

				GUI_Hub::RenderPanels();
				GUI_Hub::RenderButtons();
				CreateProject::RenderCreateProjectWindow();
				RemoveProject::RenderRemoveProjectWindow();

				ImGui::Render();
			}

			ImDrawData* drawData = ImGui::GetDrawData();
			if (drawData != nullptr)
			{
				ImGui_ImplOpenGL3_RenderDrawData(drawData);
			}
		}
	}

	void GUI_Hub::RenderPanels()
	{
		glfwGetFramebufferSize(Render::window, &framebufferWidth, &framebufferHeight);

		ImGuiWindowFlags mainWindowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoSavedSettings;

		ImGui::Begin("Panels", NULL, mainWindowFlags);

		ImGui::SetWindowPos(ImVec2(200, 0));
		ImGui::SetWindowSize(ImVec2(static_cast<float>(framebufferWidth) - 200, static_cast<float>(framebufferHeight)));

		int panelHeightInt = static_cast<int>(panelHeight);
		int panelSpacingInt = static_cast<int>(panelSpacing);
		int height = (panelHeightInt + panelSpacingInt) * static_cast<int>(files.size());
		if (height < framebufferHeight - 20) height = framebufferHeight - 20;
		ImGui::BeginChild("ScrollingRegion", ImVec2(static_cast<float>(framebufferWidth), static_cast<float>(height)), true, ImGuiWindowFlags_HorizontalScrollbar);

		ImVec2 nextPanelPos = ImGui::GetCursorScreenPos();

		if (files.size() > 0)
		{
			for (const auto& file : files)
			{
				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoCollapse
					| ImGuiWindowFlags_NoTitleBar
					| ImGuiWindowFlags_NoResize
					| ImGuiWindowFlags_NoMove
					| ImGuiWindowFlags_NoSavedSettings;

				ImGui::SetNextWindowPos(nextPanelPos);

				ImGui::Begin(file.c_str(), NULL, windowFlags);

				ImGui::SetWindowSize(ImVec2(static_cast<float>(framebufferWidth) - 235, 100));

				if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
					&& !CreateProject::renderCreateProjectWindow)
				{
					ImGui::SetWindowFocus();
				}

				string fileName = path(file).stem().string();
				ImGui::Text("%s", fileName.c_str());

				if (ImGui::Button("Launch", ImVec2(200, 50)))
				{
					if (IsValidEnginePath(Hub::enginePath.string()))
					{
						GUI_Hub::RunProject(file);
					}
				}

				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

				if (ImGui::Button("Remove", ImVec2(200, 50)))
				{
					if (!CreateProject::renderCreateProjectWindow
						&& !RemoveProject::renderRemoveProjectWindow)
					{
						RemoveProject::confirmFileName = fileName;
						RemoveProject::confirmFilePath = file;
						RemoveProject::renderRemoveProjectWindow = true;
					}
				}

				ImGui::End();

				nextPanelPos.y += panelHeight + panelSpacing;
			}
		}

		ImGui::EndChild();

		ImGui::End();
	}

	void GUI_Hub::RenderButtons()
	{
		glfwGetFramebufferSize(Render::window, &framebufferWidth, &framebufferHeight);

		ImGui::SetNextWindowSize(ImVec2(200, static_cast<float>(framebufferHeight)));
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

		ImGuiWindowFlags mainWindowFlags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoSavedSettings;

		ImGui::Begin("Buttons", NULL, mainWindowFlags);

		if (ImGui::Button("Create new project", ImVec2(185, 50)))
		{
			GUI_Hub::NewProject();
		}

		ImGui::Dummy(ImVec2(0.0f, 15.0f));

		if (ImGui::Button("Set project folder", ImVec2(185, 50)))
		{
			GUI_Hub::SetProjectsFolder();
		}

		ImGui::Dummy(ImVec2(0.0f, 15.0f));

		if (ImGui::Button("Set engine path", ImVec2(185, 50)))
		{
			GUI_Hub::SetEnginePath();
		}

		ImGui::End();
	}

	void GUI_Hub::NewProject()
	{
		if (CreateProject::renderCreateProjectWindow
			|| RemoveProject::renderRemoveProjectWindow) return;

		if (Hub::projectsFolderPath == "")
		{
			Hub::CreateWarningPopup("Error: Projects folder has not been assigned so a new project cannot be created!");
			return;
		}

		CreateProject::renderCreateProjectWindow = true;
	}

	void GUI_Hub::SetProjectsFolder()
	{
		if (CreateProject::renderCreateProjectWindow
			|| RemoveProject::renderRemoveProjectWindow) return;

		string filePath = SelectWithExplorer(SelectType::folder);

		Hub::projectsFolderPath = path(filePath);

		UpdateFileList();

		Hub::SaveConfigFile();
	}

	void GUI_Hub::SetEnginePath()
	{
		if (CreateProject::renderCreateProjectWindow
			|| RemoveProject::renderRemoveProjectWindow) return;

		string filePath = SelectWithExplorer(SelectType::engine_path);

		if (filePath.empty())
		{
			cout << "Cancelled engine selection.\n\n";
			return;
		}

		path enginePath(filePath);
#ifdef _WIN32
		if (enginePath.stem().string() != "Elypso engine"
			|| enginePath.extension().string() != ".exe")
		{
			Hub::CreateWarningPopup(("Error: Path " + filePath + " does not lead to Elypso engine.exe!").c_str());
			return;
		}
#elif __linux__
		if (enginePath.stem().string() != "Elypso engine")
		{
			Hub::CreateWarningPopup(("Error: Path " + filePath + " does not lead to Elypso engine.exe!").c_str());
			return;
		}
#endif

		Hub::enginePath = filePath;

		Hub::SaveConfigFile();
	}

	void GUI_Hub::RemoveProject(const string& projectPath)
	{
		if (!exists(projectPath))
		{
			Hub::CreateWarningPopup(("Error: Tried to remove '" + projectPath + "' but it has already been removed!").c_str());
			UpdateFileList();
			return;
		}

		string projectsFile = (path(Hub::docsPath).parent_path() / "Elypso engine" / "project.txt").string();
		if (exists(projectsFile)) remove(projectsFile);

		remove_all(projectPath);
		UpdateFileList();

		cout << "Removed existing project '" << projectPath << "'\n\n";
	}

	bool GUI_Hub::IsValidEnginePath(const string& enginePath)
	{
		if (Hub::enginePath == "")
		{
			Hub::CreateWarningPopup("Error: Couldn't run engine because no valid path could be loaded!");
			return false;
		}

		if (!exists(Hub::enginePath))
		{
			Hub::CreateWarningPopup(("Error: Tried to run '" + (Hub::enginePath).string() + "' but it doesn't exist!").c_str());
			return false;
		}

#ifdef _WIN32
		if (Hub::enginePath.stem().string() != "Elypso engine"
			|| Hub::enginePath.extension().string() != ".exe")
		{
			cout << "\n\n";
			Hub::CreateWarningPopup(("Error: Path '" + (Hub::enginePath).string() + "' does not lead to Elypso engine.exe!").c_str());
			return false;
		}
#elif __linux__
		if (Hub::enginePath.stem().string() != "Elypso engine")
		{
			Hub::CreateWarningPopup(("Error: Path '" + (Hub::enginePath).string() + "' does not lead to Elypso engine.exe!").c_str());
			return false;
		}
#endif

		return true;
	}

	void GUI_Hub::RunProject(const string& targetProject)
	{
		if (CreateProject::renderCreateProjectWindow
			|| RemoveProject::renderRemoveProjectWindow) return;

		if (!exists(targetProject))
		{
			cout << "Error: Trying to run project that no longer exists!\n";
			UpdateFileList();
			return;
		}

		GUI_Hub::RunApplication(
			Hub::enginePath.parent_path().string(),
			Hub::enginePath.string());
	}

	void GUI_Hub::RunApplication(const string& parentPath, const string& exePath)
	{
#ifdef _WIN32
		wstring wParentFolderPath(parentPath.begin(), parentPath.end());
		wstring wExePath(exePath.begin(), exePath.end());
		string commands{};
		wstring wCommands(commands.begin(), commands.end());

		//initialize structures for process creation
		STARTUPINFOW si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		si.cb = sizeof(si);

		// Create the new process
		if (!CreateProcessW
		(
			wExePath.c_str(),          //path to the executable
			const_cast<LPWSTR>(wCommands.c_str()), //command line arguments
			nullptr,                   //process handle not inheritable
			nullptr,                   //thread handle not inheritable
			FALSE,                     //handle inheritance
			0,                         //creation flags
			nullptr,                   //use parent's environment block
			wParentFolderPath.c_str(), //use parent's starting directory
			&si,                       //pointer to STARTUPINFO structure
			&pi                        //pointer to PROCESS_INFORMATION structure
		))
		{
			//retrieve the error code and print a descriptive error message
			LPVOID lpMsgBuf = nullptr;
			DWORD dw = GetLastError();
			FormatMessageW(
				FORMAT_MESSAGE_ALLOCATE_BUFFER
				| FORMAT_MESSAGE_FROM_SYSTEM
				| FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPWSTR)&lpMsgBuf, 0, nullptr);
			std::wcout << L"Error: " << reinterpret_cast<LPCWSTR>(lpMsgBuf) << L"\n\n";
			LocalFree(lpMsgBuf);
		}

		// Close process and thread handles
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
#elif __linux__
		pid_t pid = fork();
		if (pid < 0)
		{
			perror("Error during fork");
			return;
		}

		if (pid == 0)
		{
			//detach from parent session
			if (setsid() < 0)
			{
				perror("Error creating new session");
				exit(EXIT_FAILURE);
			}

			//change working dir to game exe folder
			if (chdir(parentPath.c_str()) != 0)
			{
				perror("Error changing directory");
				exit(EXIT_FAILURE);
			}

			//empty arguments
			char* const argv[] = { const_cast<char*>(exePath.c_str()), nullptr };

			//replace child with executable
			execv(exePath.c_str(), argv);

			perror("Error launching exe process");
			exit(EXIT_FAILURE);
		}
#endif
	}

	void GUI_Hub::UpdateFileList()
	{
		files.clear();
		if (Hub::projectsFolderPath != "")
		{
			for (const auto& entry : directory_iterator(Hub::projectsFolderPath))
			{
				if (is_directory(entry))
				{
					files.push_back(entry.path().string());
				}
			}
		}
	}

	string GUI_Hub::SelectWithExplorer(SelectType selectType)
	{
#ifdef _WIN32
		//initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr))
		{
			cout << "Error: Failed to initialize COM!\n\n";
			return "";
		}

		//create an instance of the File Open dialog
		IFileOpenDialog* pFileOpen = nullptr;
		hr = CoCreateInstance(
			CLSID_FileOpenDialog,
			NULL,
			CLSCTX_ALL,
			IID_IFileOpenDialog,
			reinterpret_cast<void**>(&pFileOpen));
		if (FAILED(hr))
		{
			cout << "Error: Failed to create File Open dialog!\n\n";
			CoUninitialize();
			return "";
		}

		if (selectType == SelectType::folder)
		{
			//restrict the selection to folders only
			DWORD dwOptions;
			hr = pFileOpen->GetOptions(&dwOptions);
			if (SUCCEEDED(hr))
			{
				hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
				if (FAILED(hr))
				{
					cout << "Error: Failed to set options!\n\n";
					pFileOpen->Release();
					CoUninitialize();
					return "";
				}
			}
			else
			{
				cout << "Error: Failed to get options!\n\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		else if (selectType == SelectType::engine_path)
		{
			//restrict file selection to .exe only
			COMDLG_FILTERSPEC filterSpec[] = { { L"Executables", L"*.exe"} };
			hr = pFileOpen->SetFileTypes(1, filterSpec);
			if (FAILED(hr))
			{
				cout << "Error: Failed to set file filter!\n\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		else if (selectType == SelectType::txt_file)
		{
			//restrict file selection to .txt only
			COMDLG_FILTERSPEC filterSpec[] = { { L"Scene files", L"*.txt"} };
			hr = pFileOpen->SetFileTypes(1, filterSpec);
			if (FAILED(hr))
			{
				cout << "Error: Failed to set file filter!\n\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}

		//show the File Open dialog
		hr = pFileOpen->Show(NULL);
		if (FAILED(hr))
		{
			cout << "Error: Failed to show dialog!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//get the result of the user's selection
		IShellItem* pItem;
		hr = pFileOpen->GetResult(&pItem);
		if (FAILED(hr))
		{
			cout << "Error: Failed to retrieve result!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//get the path pf the selected file or folder
		PWSTR filePath;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
		if (FAILED(hr))
		{
			cout << "Error: Failed to retrieve path!\n\n";
			pItem->Release();
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}

		//convert the wide string to a string
		wstring ws(filePath);

		//get the required buffer size
		int size_needed = WideCharToMultiByte(
			CP_UTF8,
			0,
			ws.c_str(),
			static_cast<int>(ws.length()),
			NULL,
			0,
			NULL,
			NULL);

		//convert wide string to utf-8 encoded narrow string
		string narrowPath(size_needed, 0);
		WideCharToMultiByte(
			CP_UTF8,
			0,
			ws.c_str(),
			static_cast<int>(ws.length()),
			&narrowPath[0],
			size_needed,
			NULL,
			NULL);

		//free memory allocated for filePath
		CoTaskMemFree(filePath);

		//release the shell item
		pItem->Release();

		//release the file open dialog
		pFileOpen->Release();

		//uninitialze COM
		CoUninitialize();

		return narrowPath;
#elif __linux__
		string command = "zenity --file-selection";

		switch (selectType)
		{
		case SelectType::txt_file:
			command += " --file-filter='*.txt'";
			break;
		case SelectType::engine_path:
			command += " --file-filter='All files | *'";
			break;
		case SelectType::folder:
			command = "zenity --file-selection --directory";
			break;
		}

		//execute the command and capture the output
		array<char, 128> buffer{};
		string result;
		FILE* pipe = popen(command.c_str(), "r");
		if (!pipe)
		{
			Hub::CreateErrorPopup("Failed to open file dialog!");
		}
		while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
		{
			result += buffer.data();
		}
		pclose(pipe);

		//remove trailing newline from result
		if (!result.empty()
			&& result.back() == '\n')
		{
			result.pop_back();
		}

		//check if selected file is executable
		if (selectType == SelectType::engine_path)
		{
			if (access(result.c_str(), X_OK) != 0)
			{
				//Hub::CreateErrorPopup("Selected file is not executable!");
				cout << "not executable!\n";
				return "";
			}
		}

		return result;
#endif
	}

	void GUI_Hub::Shutdown()
	{
		if (isImguiInitialized)
		{
			isImguiInitialized = false;

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	}
}
