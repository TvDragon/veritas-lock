#include "main_view.h"

MainView::MainView(struct DataContext* ptrDataContext, DataHandler* ptrDataHandler)
	:ptrDataContext(ptrDataContext)
	, ptrDataHandler(ptrDataHandler)
{

}

MainView::~MainView() {
	ptrDataContext->selectedLogin.clear();
	memset(ptrDataContext->accountBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
	memset(ptrDataContext->typeBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
	memset(ptrDataContext->usernameBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
	memset(ptrDataContext->emailBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
	memset(ptrDataContext->passwordBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
	memset(ptrDataContext->websiteBuff, '\0', WEBSITE_MAX_LENGTH);
	memset(ptrDataContext->notesBuff, '\0', NOTES_MAX_LENGTH);
	memset(ptrDataContext->userBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
}

void MainView::DisplayMain() {
	// Position the window directly below the menu bar
	ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));

	// Set window size to take up the entire application window below the menu bar
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - ImGui::GetFrameHeight()));

	// Add padding inside the window
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 10)); // Adjust padding as needed

	// Borderless non-resizeable window
	ImGui::Begin("Logins", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	ImGui::PopStyleVar();	// Restore previous padding style
	
	// Display search bar
	SearchBar();

	// Space between search bar and and the next section
	ImGui::Spacing();
	
	DisplayLogins(ptrDataHandler->GetLogins());

	ImGui::End();
}

void MainView::SearchBar() {
// Set the width of the next item to the available content region width
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Delete Group").x - ImGui::GetStyle().ItemSpacing.x);
	std::stringstream ss;
	if (ptrDataHandler->GetType().empty()) {
		ss << "No logins to search for.";
	} else {
		ss << "Search for " << ptrDataHandler->GetType() << "...";
	}
	ImGui::InputTextWithHint("##search", ss.str().c_str(), searchBuff, IM_ARRAYSIZE(searchBuff));
	ss.str("");
	ImGui::SameLine();
	if (ImGui::Button("Delete Group")) {
		ImGui::OpenPopup("Delete Group Confirmation");
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Delete Group Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you wish to delete all logins for this group?");
		
		float modalWidth = ImGui::GetContentRegionAvail().x;
		float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;
		
		if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
			bool success = ptrDataHandler->DeleteType(ptrDataHandler->GetType());
			if (success) {
				std::vector<std::string> types = ptrDataHandler->GetTypes();
				if (types.size() != 0) {
					ptrDataHandler->SetType(types[0]);
					ptrDataHandler->GetLogins(types[0]);
				} else {
					ptrDataHandler->SetType("");
				}
				ImGui::OpenPopup("Delete Group Success");
				ptrDataContext->selectedLogin.clear();
				ptrDataContext->selectedLoginIdx = -1;
			} else {
				ImGui::OpenPopup("Delete Group Fail");
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Delete Group Success", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Successfully Deleted Logins For Group.");

		// Set button width to match modal width
		float buttonWidth = ImGui::GetContentRegionAvail().x;

		if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	} else if (ImGui::BeginPopupModal("Delete Group Fail", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Failed To Delete Logins For Group.");

		// Set button width to match modal width
		float buttonWidth = ImGui::GetContentRegionAvail().x;

		if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void MainView::DisplayLoginBtn(std::map<std::string, std::string> login,
							const char* loginDetails, float loginsSecWidth,
							int idx, int numLogins) {
	// Get the starting position for the background rectangle
	ImVec2 start = ImGui::GetCursorScreenPos();
	int btnWidth = loginsSecWidth - 30;
	int btnHeight = ImGui::CalcTextSize(loginDetails).y + 3;
	ImVec2 end = ImVec2(start.x + btnWidth, start.y + btnHeight);

	// Adjust padding values
	float paddingX = 10.0f;
	
	// Set background color
	ImU32 bgColor = IM_COL32(169, 180, 194, 255); // Adjust RGBA as needed

	if (ptrDataContext->selectedLoginIdx == idx) {
		bgColor = IM_COL32(84, 84, 84, 255);
	}
	// Draw the background color rectangle
	ImGui::GetWindowDrawList()->AddRectFilled(start, end, bgColor);

	// Move cursor to ensure selectable is drawn inside the padded background
	ImGui::SetCursorScreenPos(ImVec2(start.x + paddingX, start.y));

	if (ImGui::Selectable(loginDetails, false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(btnWidth, 0))) {
		// Handle button click here
		ptrDataContext->selectedLogin["id"] = login["id"];
		ptrDataContext->selectedLogin["type"] = login["type"];
		ptrDataContext->selectedLogin["account"] = login["account"];
		ptrDataContext->selectedLogin["username"] = login["username"];
		ptrDataContext->selectedLogin["email"] = login["email"];
		ptrDataContext->selectedLogin["password"] = login["password"];
		ptrDataContext->selectedLogin["notes"] = login["notes"];
		ptrDataContext->selectedLogin["website"] = login["website"];
		ptrDataContext->selectedLogin["user"] = login["user"];
		ptrDataContext->selectedLoginIdx = idx;
		ptrDataContext->subView = LOGIN_DISPLAY_VIEW;
	}

	// Restore cursor position to avoid layout issues
	ImGui::SetCursorScreenPos(ImVec2(start.x, end.y));

	if (idx != numLogins - 1) {
		ImGui::Spacing();
	}
}

std::string toLower(const std::string& str) {
	std::string lowerStr = str; // Create a copy of the input string
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
				[](unsigned char c) { return std::tolower(c); });
	return lowerStr;
}

void MainView::DisplayLogins(std::vector<std::map<std::string, std::string>> logins) {
	float totalWidth = ImGui::GetContentRegionAvail().x;
	float loginsSecWidth = totalWidth * 0.2f;
	float loginSecWidth = totalWidth - loginsSecWidth;

	// Convert char buffer to std::string
	std::string bufferStr(searchBuff);

	// Display logins section
	ImGui::BeginChild("FirstSection", ImVec2(loginsSecWidth, 0), true);
	std::stringstream ss;
	for (int i = 0; i < logins.size(); i++) {
		std::map<std::string, std::string> login = logins[i];
		ss << login["account"] << "\n";
		if (login["email"].empty()) {
			ss << login["username"];
		} else {
			ss << login["email"];
		}

		// Convert both strings to lower case
		std::string lowerEmail = toLower(login["email"]);
		std::string lowerAccount = toLower(login["account"]);
		std::string lowerUsername = toLower(login["username"]);
		std::string lowerBufferStr = toLower(bufferStr);

		if (searchBuff[0] != '\0') {
			if (lowerEmail.find(lowerBufferStr) != std::string::npos ||
					lowerAccount.find(lowerBufferStr) != std::string::npos ||
					lowerUsername.find(lowerBufferStr) != std::string::npos) {
				DisplayLoginBtn(logins[i], ss.str().c_str(), loginsSecWidth, i, logins.size());
			}	
		} else {
			DisplayLoginBtn(logins[i], ss.str().c_str(), loginsSecWidth, i, logins.size());
		}

		ss.str("");
	}
	ImGui::EndChild();

	ImGui::SameLine();  // Keep the next item on the same row

	ImGui::BeginChild("SecondSection", ImVec2(loginSecWidth, 0), true);

	switch (ptrDataContext->subView) {
		case LOGIN_DISPLAY_VIEW:
			// Display login details section
			DisplayLogin(loginSecWidth);
			break;
		case EDIT_LOGIN_DISPLAY_VIEW:
			EditLogin();
			break;
		default:
			break;
	}
	ImGui::EndChild();	
}

void MainView::DisplayLogin(float loginSecWidth) {
	if (!ptrDataContext->selectedLogin.empty() && ptrDataContext->selectedLoginIdx != -1) {
		ImGui::Text(ptrDataContext->selectedLogin["account"].c_str());

		// Move cursor to the right for the buttons
		float buttonWidth = ImGui::CalcTextSize("Edit Login").x +
							ImGui::CalcTextSize("Delete Login").x +
							ImGui::GetStyle().ItemSpacing.x * 2;	// Calculate total width of buttons including padding between them
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonWidth);
		if (ImGui::Button("Edit Login")) {
			ptrDataContext->subView = EDIT_LOGIN_DISPLAY_VIEW;
			ptrDataContext->isInitialEdit = true;
		}	

		ImGui::SameLine();
		if (ImGui::Button("Delete Login")) {
			ImGui::OpenPopup("Delete Login Confirmation");
		}

		if (ImGui::BeginPopupModal("Delete Login Confirmation",  NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Are you sure you wish to delete this login?");

			float modalWidth = ImGui::GetContentRegionAvail().x;
			float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;
			
			if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
				std::string id = ptrDataContext->selectedLogin["id"];
				bool success = ptrDataHandler->DeleteLogin(std::stoi(id), false);
				if (success) {
					ptrDataContext->selectedLogin.clear();
					ptrDataContext->selectedLoginIdx = -1;
					ImGui::OpenPopup("Delete Login Success");
				} else {
					ImGui::OpenPopup("Delete Login Failed");
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Delete Login Success", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Successfully Deleted Login.");

			// Set button width to match modal width
			float buttonWidth = ImGui::GetContentRegionAvail().x;

			if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		} else if (ImGui::BeginPopupModal("Delete Login Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Failed To Delete Login.");

			// Set button width to match modal width
			float buttonWidth = ImGui::GetContentRegionAvail().x;

			if (ImGui::Button("OK", ImVec2(buttonWidth, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		std::stringstream ss;
		std::string username = ptrDataContext->selectedLogin["username"];
		std::string email = ptrDataContext->selectedLogin["email"];
		std::string password = ptrDataContext->selectedLogin["password"];
		std::string website = ptrDataContext->selectedLogin["website"];
		std::string notes = ptrDataContext->selectedLogin["notes"];
		ss << "Username\n";
		if (!(username.empty())) {
			ss << username << "\n";
		}
		ss << "\nEmail\n";
		if (!(email.empty())) {
			ss << email << "\n";
		}
		ss << "\nPassword\n";
		if (!(password.empty())) {
			ss << password << "\n";
		}
		ss << "\n";
		ImGui::PushStyleColor(ImGuiCol_Header, ImColor(221, 221, 221).Value);		// Menu item color
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImColor(169, 180, 194).Value);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImColor(84, 84, 84).Value);
		// ImGui::Button(ss.str().c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0));
		ImGui::BeginChild("##accountDetails", ImVec2(ImGui::GetContentRegionAvail().x,
							ImGui::CalcTextSize(ss.str().c_str()).y + 24), true,
							ImGuiWindowFlags_NoScrollbar);
		ImGui::TextUnformatted("Username\n");
		if (!(username.empty())) {
			if(ImGui::Selectable(username.c_str())) {
				ImGui::SetClipboardText(username.c_str());
			}
		}
		ImGui::TextUnformatted("\nEmail\n");
		if (!(email.empty())) {
			if(ImGui::Selectable(email.c_str())) {
				ImGui::SetClipboardText(email.c_str());
			}
		}
		ImGui::TextUnformatted("\nPassword\n");
		if (!(password.empty())) {
			if(ImGui::Selectable(password.c_str())) {
				ImGui::SetClipboardText(password.c_str());
			}
		}
		ImGui::EndChild();

		ss.str("");

		ss << "Website\n";
		if (!(website.empty())) {
			ss << website << "\n";
		}
		ss << "\n";
		ImGui::BeginChild("##website", ImVec2(ImGui::GetContentRegionAvail().x,
							ImGui::CalcTextSize(ss.str().c_str()).y + 8), true,
							ImGuiWindowFlags_NoScrollbar);
		ImGui::TextUnformatted("Website\n");
		if (!(website.empty())) {
			if(ImGui::Selectable(website.c_str())) {
				ImGui::SetClipboardText(website.c_str());
			}
		}
		ImGui::EndChild();

		ss.str("");
		ss << "Notes\n";
		if (!(notes.empty())) {
			ss << notes << "\n";
		}
		ss << "\n";

		int numLines = 0;
		std::stringstream ssNotes(notes);
		std::string t;
		while (getline(ssNotes, t, '\n')) {
			if (!t.empty()) {
				numLines++;
			}
		}

		ImGui::BeginChild("##notes", ImVec2(ImGui::GetContentRegionAvail().x,
							ImGui::CalcTextSize(ss.str().c_str()).y + (numLines * 8)), true,
							ImGuiWindowFlags_NoScrollbar);
		ss.clear();
		ss.str("");
		ss << notes;
		ImGui::TextUnformatted("Notes\n");
		if (!(notes.empty())) {
			std::string t;
			int lineIndex = 0;
			while (getline(ss, t, '\n')) {
				if (!t.empty()) {
					std::string uniqueLabel = t + "##" + std::to_string(lineIndex); // unique identifer for internal use after ## and to not displayed
					if(ImGui::Selectable(uniqueLabel.c_str())) {
						ImGui::SetClipboardText(t.c_str());
					}
				} else {
					std::string uniqueLabel = "##empty_" + std::to_string(lineIndex);
					if (ImGui::Selectable(uniqueLabel.c_str())) {
						ImGui::SetClipboardText("");
					}
				}
				lineIndex++;
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor(3);
	} else {
		// When ptrDataContext->selectedLogin is empty, render nothing
		ImGui::Text("No login selected.");
	}
}

void MainView::EditLogin() {	// Technically 2nd screen so may need separate class
	if (ptrDataContext->isInitialEdit) {
		memset(ptrDataContext->usernameBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		memset(ptrDataContext->emailBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		memset(ptrDataContext->passwordBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		memset(ptrDataContext->websiteBuff, '\0', WEBSITE_MAX_LENGTH);
		memset(ptrDataContext->notesBuff, '\0', NOTES_MAX_LENGTH);
		memset(ptrDataContext->userBuff, '\0', DEFAULT_LOGINS_MAX_LENGTH);
		strncpy(ptrDataContext->usernameBuff, ptrDataContext->selectedLogin["username"].c_str(), strlen(ptrDataContext->selectedLogin["username"].c_str()));
		strncpy(ptrDataContext->emailBuff, ptrDataContext->selectedLogin["email"].c_str(), strlen(ptrDataContext->selectedLogin["email"].c_str()));
		strncpy(ptrDataContext->passwordBuff, ptrDataContext->selectedLogin["password"].c_str(), strlen(ptrDataContext->selectedLogin["password"].c_str()));
		strncpy(ptrDataContext->websiteBuff, ptrDataContext->selectedLogin["website"].c_str(), strlen(ptrDataContext->selectedLogin["website"].c_str()));
		strncpy(ptrDataContext->notesBuff, ptrDataContext->selectedLogin["notes"].c_str(), strlen(ptrDataContext->selectedLogin["notes"].c_str()));
		strncpy(ptrDataContext->userBuff, ptrDataContext->selectedLogin["user"].c_str(), strlen(ptrDataContext->selectedLogin["user"].c_str()));
		ptrDataContext->isInitialEdit = false;
	}

	//ImGui::SetNextWindowPos(ImVec2(windowWidth / 2, windowHeight / 2));
	ImGui::Text(ptrDataContext->selectedLogin["account"].c_str());

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

	if (ImGui::Button("Save")) {
		ImGui::OpenPopup("Save Changes Confirmation");
	}

	float buttonWidth = ImGui::CalcTextSize("Close").x;
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonWidth);
	if (ImGui::Button("Close")) {
		ImGui::OpenPopup("Close Edit Confirmation");
	}

	if (ImGui::BeginPopupModal("Save Changes Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you wish to save these changes?");

		std::string account = ptrDataContext->selectedLogin["account"];
		std::string username = ptrDataContext->usernameBuff;
		std::string type = ptrDataContext->selectedLogin["type"];
		std::string email = ptrDataContext->emailBuff;
		std::string password = ptrDataContext->passwordBuff;
		std::string notes = ptrDataContext->notesBuff;
		std::string website = ptrDataContext->websiteBuff;
		std::string user = ptrDataContext->userBuff;

		float modalWidth = ImGui::GetContentRegionAvail().x;
		float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

		if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
			std::string id = ptrDataContext->selectedLogin["id"];
			ptrDataHandler->UpdateLogin(std::stoi(id), account, type, username, email, password, notes, website, user);
			ptrDataContext->selectedLogin["username"] = ptrDataContext->usernameBuff;
			ptrDataContext->selectedLogin["email"] = ptrDataContext->emailBuff;
			ptrDataContext->selectedLogin["password"] = ptrDataContext->passwordBuff;
			ptrDataContext->selectedLogin["notes"] = ptrDataContext->notesBuff;
			ptrDataContext->selectedLogin["website"] = ptrDataContext->websiteBuff;
			ptrDataContext->selectedLogin["user"] = ptrDataContext->userBuff;
			ptrDataContext->subView = LOGIN_DISPLAY_VIEW;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	} else if (ImGui::BeginPopupModal("Close Edit Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you wish to close without any changes made?");
		
		float modalWidth = ImGui::GetContentRegionAvail().x;
		float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;

		if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
			ptrDataContext->subView = LOGIN_DISPLAY_VIEW;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}