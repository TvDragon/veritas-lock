#include "master_login_view.h"

MasterLoginView::MasterLoginView(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler)
	:ptrDataContext(ptrDataContext)
	,ptrDataHandler(ptrDataHandler)
{

}

MasterLoginView::~MasterLoginView() {
	memset(passwordBuff, '\0', PASSWORD_LENGTH);
}

void MasterLoginView::DisplayMasterLoginView() {
	if (ptrDataContext->isInitialEdit) {
		memset(passwordBuff, '\0', PASSWORD_LENGTH);
		ptrDataContext->isInitialEdit = false;
	}

	// Position the window directly below the menu bar
	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));

	// Set window size to take up the entire application window below the menu bar
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - ImGui::GetFrameHeight()));

	// Add padding inside the window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 10));  // Adjust padding as needed

	ImGui::Begin("Master Login", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	ImGui::PopStyleVar();	// Restore previous padding style

	ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 textSize = ImGui::CalcTextSize("Master Login");
    ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f); // Center horizontally
	ImGui::Text("Master Login");

	std::string password = ptrDataHandler->GetMasterPassword();
	if (password.empty()) {
		ImGui::Text("Create a Master Password to login each time.\n\n");
	}

	ImGui::BeginChild("##addNewLogin", ImVec2(ImGui::GetContentRegionAvail().x,
						70), true, ImGuiWindowFlags_NoScrollbar);

	
	ImGui::Text("Password");
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##password", passwordBuff, passwordBuff, IM_ARRAYSIZE(passwordBuff));

	ImGui::EndChild();

	if (ImGui::Button("Login")) {
		if (!password.empty()) {
			if (strcmp(passwordBuff, password.c_str()) == 0) {
				ptrDataContext->view = MAIN_DISPLAY_VIEW;
				ptrDataContext->isInitialEdit = true;
			} else {
				ImGui::OpenPopup("Incorrect Password Popup");
			}
		} else {
			ImGui::OpenPopup("Login Confirmation");
		}
	}

	if (ImGui::BeginPopupModal("Login Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you wish to use this as your Master Password?");
		password = passwordBuff;

		float modalWidth = ImGui::GetContentRegionAvail().x;
		float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

		if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
			ptrDataHandler->AddMasterLogin(password);
			ptrDataContext->view = MAIN_DISPLAY_VIEW;
			ptrDataContext->isInitialEdit = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	} else if (ImGui::BeginPopupModal("Incorrect Password Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Incorrect Password. Please Try Again.");

		if (ImGui::Button("OK")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}