#include "add_login_view.h"

AddLoginView::AddLoginView(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler)
	: ptrDataContext(ptrDataContext)
	, ptrDataHandler(ptrDataHandler)
{

}

AddLoginView::~AddLoginView() {}

void AddLoginView::DisplayAddLogin() {
	if (ptrDataContext->isInitialEdit) {
		memset(ptrDataContext->accountBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		memset(ptrDataContext->typeBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		memset(ptrDataContext->usernameBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		memset(ptrDataContext->emailBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		memset(ptrDataContext->passwordBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		memset(ptrDataContext->websiteBuff, '\0', WEBSITE_MAX_LENGTH);
		memset(ptrDataContext->notesBuff, '\0', NOTES_MAX_LENGTH);
		memset(ptrDataContext->userBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		ptrDataContext->isInitialEdit = false;
	}

	// Position the window directly below the menu bar
	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));

	// Set window size to take up the entire application window below the menu bar
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - ImGui::GetFrameHeight()));

	// Add padding inside the window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 10));

	ImGui::Begin("Add Login", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	ImGui::PopStyleVar();

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 textSize = ImGui::CalcTextSize("Add New Login");
	ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);	// Center horizonally
	ImGui::Text("Add New Login");

	int contentHeight = 400 + ImGui::CalcTextSize(ptrDataContext->notesBuff).y + 5;

	ImGui::BeginChild("##addNewLogin", ImVec2(ImGui::GetContentRegionAvail().x,
						contentHeight), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::Text("Account");
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##account", ptrDataContext->accountBuff, ptrDataContext->accountBuff, IM_ARRAYSIZE(ptrDataContext->accountBuff));

	ImGui::Text("Type");
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##type", ptrDataContext->typeBuff, ptrDataContext->typeBuff, IM_ARRAYSIZE(ptrDataContext->typeBuff));

	ImGui::Text("Username");
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##username", ptrDataContext->usernameBuff, ptrDataContext->usernameBuff, IM_ARRAYSIZE(ptrDataContext->usernameBuff));

	ImGui::Text("Email");
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##email", ptrDataContext->emailBuff, ptrDataContext->emailBuff, IM_ARRAYSIZE(ptrDataContext->emailBuff));

	ImGui::Text("Password");
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##password", ptrDataContext->passwordBuff, ptrDataContext->passwordBuff, IM_ARRAYSIZE(ptrDataContext->passwordBuff));
	
	ImGui::Text("Website");
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##website", ptrDataContext->websiteBuff, ptrDataContext->websiteBuff, IM_ARRAYSIZE(ptrDataContext->websiteBuff));
	
	float inputHeight = ImGui::CalcTextSize(ptrDataContext->notesBuff).y + 8.0f;	// padding so can see all text clearly
	ImGui::Text("Notes");
	ImGui::InputTextMultiline("##notes", ptrDataContext->notesBuff, IM_ARRAYSIZE(ptrDataContext->notesBuff), ImVec2(ImGui::GetContentRegionAvail().x, inputHeight));
	
	ImGui::Text("User");
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##user", ptrDataContext->userBuff, ptrDataContext->userBuff, IM_ARRAYSIZE(ptrDataContext->userBuff));
	
	ImGui::EndChild();

	if (ImGui::Button("Add New Login")) {
		if (strlen(ptrDataContext->accountBuff) == 0 ||
				strlen(ptrDataContext->typeBuff) == 0 ||
				strlen(ptrDataContext->userBuff) == 0) {
			ImGui::OpenPopup("Empty Field New Login");
		} else {
			ImGui::OpenPopup("Add Login Confirmation");
		}
	}

	float buttonWidth = ImGui::CalcTextSize("Close").x;
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonWidth);
	if (ImGui::Button("Close")) {
		ImGui::OpenPopup("Cancel New Login Confirmation");
	}

	if (ImGui::BeginPopupModal("Add Login Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you wish to add new login?");

		std::string account = ptrDataContext->accountBuff;
		std::string username = ptrDataContext->usernameBuff;
		std::string type = ptrDataContext->typeBuff;
		std::string email = ptrDataContext->emailBuff;
		std::string password = ptrDataContext->passwordBuff;
		std::string notes = ptrDataContext->notesBuff;
		std::string website = ptrDataContext->websiteBuff;
		std::string user = ptrDataContext->userBuff;

		float modalWidth = ImGui::GetContentRegionAvail().x;
		float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

		if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
			std::string output = ptrDataHandler->AddLogin(account, type, username, email, password, notes, website, user);
			if (output.empty()) {
				ptrDataHandler->GetLogins(type);
				ptrDataContext->view = MAIN_DISPLAY_VIEW;
				ptrDataContext->isInitialEdit = true;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	} else if (ImGui::BeginPopupModal("Cancel New Login Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you wish to close without adding new login?");

		float modalWidth = ImGui::GetContentRegionAvail().x;
		float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

		if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
			ptrDataContext->view = MAIN_DISPLAY_VIEW;
			ptrDataContext->isInitialEdit = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	} else if (ImGui::BeginPopupModal("Empty Field New Login", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Cannot have empty fields for Account, Types and User");

		if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}