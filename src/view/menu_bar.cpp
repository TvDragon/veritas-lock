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
	}
	ImGui::PopStyleColor(5);
}