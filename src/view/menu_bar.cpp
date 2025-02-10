#include "menu_bar.h"

MenuBar::MenuBar(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler)
	:ptrDataContext(ptrDataContext)
	, ptrDataHandler(ptrDataHandler)
{

}

MenuBar::~MenuBar() {
	errorLogins.clear();
	failedReadLogins.clear();
}

void MenuBar::DisplayMenuBar() {
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImColor(210, 210, 210).Value);	// Menu bar color
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImColor(210, 210, 210).Value);		// Menu (Groups) background color
	ImGui::PushStyleColor(ImGuiCol_Header, ImColor(210, 210, 210).Value);		// Menu item color
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImColor(84, 84, 84).Value);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImColor(169, 180, 194).Value);
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Account")) {
			if (ImGui::BeginMenu("Groups")) {
				std::vector<std::string> types = ptrDataHandler->GetTypes();
				for (int i = 0; i < types.size(); i++) {
					if (ImGui::MenuItem(types[i].c_str())) {
						ptrDataHandler->SetType(types[i]);
						ptrDataHandler->GetLogins(types[i]);
						ptrDataContext->selectedLoginIdx = -1;
						ptrDataContext->selectedLogin.clear();
						ptrDataContext->view = MAIN_VIEW;
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Users")) {
				std::vector<std::string> users = ptrDataHandler->GetUsers();
				for (int i = 0; i < users.size(); i++) {
					if (ImGui::MenuItem(users[i].c_str())) {
						ptrDataHandler->SetUser(users[i]);
						std::vector<std::string> types = ptrDataHandler->GetTypes();
						if (types.size() != 0) {
							ptrDataHandler->SetType(types[0]);
							ptrDataHandler->GetLogins(types[0]);
						} else {
							ptrDataHandler->SetType("");
							ptrDataHandler->GetLogins("");
						}
						ptrDataContext->selectedLoginIdx = -1;
						ptrDataContext->selectedLogin.clear();
						ptrDataContext->view = MAIN_VIEW;
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Trash")) {
				ptrDataContext->view = TRASH_VIEW;
				ptrDataHandler->GetUpdatedTrashColumnNames();
				ptrDataHandler->GetUpdatedTrashLogins();
			}
			if (ImGui::MenuItem("Add Login")) {
				ptrDataContext->view = ADD_LOGIN_VIEW;
				ptrDataContext->isInitialEdit = true;
			}
			if (ImGui::MenuItem("Import")) {
				m_fileDialogOpen = true;
				m_fileDialogInfo.type = ImGuiFileDialogType_ReadFile;
				m_fileDialogInfo.title = "Import Logins";
				m_fileDialogInfo.fileName = "sample-logins.json";
				m_fileDialogInfo.directoryPath = std::filesystem::current_path();
			}
			if (ImGui::MenuItem("Export")) {
				m_fileDialogOpen = true;
				m_fileDialogInfo.type = ImGuiFileDialogType_WriteFile;
				m_fileDialogInfo.title = "Export Logins";
				m_fileDialogInfo.fileName = "logins.json";
				m_fileDialogInfo.directoryPath = std::filesystem::current_path();
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Home")) {
			ptrDataContext->view = MAIN_VIEW;
			ptrDataContext->isInitialEdit = true;
		}
		ImGui::EndMainMenuBar();
	}
	if (ImGui::FileDialog(&m_fileDialogOpen, &m_fileDialogInfo)) {
		// Result path in: m_fileDialogInfo.resultPath

		if (m_fileDialogInfo.resultPath.extension() == ".json") {
			if (m_fileDialogInfo.type == ImGuiFileDialogType_ReadFile) {
				std::vector<std::vector<std::string>> logins;
				int numLogins = 0;
				errorLogins.clear();
				failedReadLogins.clear();
				ReadFile(m_fileDialogInfo.resultPath, logins, failedReadLogins);
				numLogins = logins.size();
				for (int i = 0; i < numLogins; i++) {
					std::string output = ptrDataHandler->AddLogin(logins[i][0],
											logins[i][1], logins[i][2],
											logins[i][3], logins[i][4],
											logins[i][5], logins[i][6],
											logins[i][7]);
					if (!output.empty()) {
						errorLogins.push_back(output);
					}
				}

				logins.clear();

				if (errorLogins.size() == 0 && failedReadLogins.size() == 0) {
					ImGui::OpenPopup("Import Logins Success");
				} else {
					ImGui::OpenPopup("Import Logins Failed");
				}
			} else if (m_fileDialogInfo.type == ImGuiFileDialogType_WriteFile) {
				std::vector<std::map<std::string, std::string>> allLogins = ptrDataHandler->GetAllLogins();
				bool success = WriteToFile(m_fileDialogInfo.resultPath, allLogins);
				if (success) {
					ImGui::OpenPopup("Export Logins Success");
				} else {
					ImGui::OpenPopup("Export Logins Failed");
				}
				allLogins.clear();
			}
		} else {
			ImGui::OpenPopup("Wrong File Type");
		}
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Import Logins Success", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		std::vector<std::string> users = ptrDataHandler->GetUsers();
		if (users.size() != 0) {
			ptrDataHandler->SetUser(users[0]);
		}
		std::vector<std::string> types = ptrDataHandler->GetTypes();
		if (types.size() != 0) {
			ptrDataHandler->GetLogins(types[0]);
			ptrDataContext->selectedLoginIdx = -1;
			ptrDataContext->selectedLogin.clear();
		}
		ImGui::Text("Successfully Imported Logins.");

		// Set button width to match modal width
		float buttonWidth = ImGui::GetContentRegionAvail().x;

		if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	} else if (ImGui::BeginPopupModal("Import Logins Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		std::stringstream ss;
		if (errorLogins.size() != 0) {
			ss << "Error occurred importing logins::\n\n";
			for (int i = 0; i < errorLogins.size(); i++) {
				ss << errorLogins[i] << "\n";
			}
		}
		if (failedReadLogins.size() != 0) {
			if (errorLogins.size() != 0) {
				ss << "\n";
			}
			ss << "Error reading in logins:\n\n";
			for (int i = 0; i < failedReadLogins.size(); i++) {
				ss << failedReadLogins[i] << "\n";
			}
		}
		ImGui::Text(ss.str().c_str());

		// Set button width to mach modal width
		float buttonWidth = ImGui::GetContentRegionAvail().x;

		if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	} else if (ImGui::BeginPopupModal("Wrong File Type", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Failed to import logins as must be JSON file type.");

		// Set button width to match modal with
		float buttonWidth = ImGui::GetContentRegionAvail().x;

		if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}  else if (ImGui::BeginPopupModal("Export Logins Success", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Successfully Exported Logins.");
		
		// Set button width to match modal width
		float buttonWidth = ImGui::GetContentRegionAvail().x;
		
		if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}  else if (ImGui::BeginPopupModal("Export Logins Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Error Occurred Exporting Logins.");

		// Set button width to match modal width
		float buttonWidth = ImGui::GetContentRegionAvail().x;

		if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(5);
}