/*
	Copyright 2020 Limeoats
	Original project: https://github.com/Limeoats/L2DFileDialog

	Changes by Vladimir Sigalkin
	Original project: https://github.com/Iam1337/ImGui-FileDialog

	Changes by Tom Stanczyk
	Original project: https://github.com/74Labs/ImGuiFileDialog

	Changed by Tony Vo
*/

#include <chrono>
#include <string>
#include <filesystem>
#include <sstream>
#include <cstring>

#include <imgui.h>

#include "imgui_filedialog.h"

using namespace std::chrono_literals;

typedef int ImGuiFileDialogSortOrder;

enum ImGuiFileDialogSortOrder_
{
	ImGuiFileDialogSortOrder_Up,
	ImGuiFileDialogSortOrder_Down,
	ImGuiFileDialogSortOrder_None
};

void RefreshInfo(ImFileDialogInfo *dialogInfo)
{
	dialogInfo->refreshInfo = false;
	dialogInfo->currentIndex = 0;
	dialogInfo->currentFiles.clear();
	dialogInfo->currentDirectories.clear();

	for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(dialogInfo->directoryPath))
	{
		if (entry.is_directory())
		{
			dialogInfo->currentDirectories.push_back(entry);
		}
		else
		{
			dialogInfo->currentFiles.push_back(entry);
		}

		// To dipslay only .json files check if: entry.path().extension() == ".json"
	}
}

bool ImGui::FileDialog(bool *open, ImFileDialogInfo *dialogInfo)
{
	if (!*open)
		return false;

	static float initialSpacingColumn0 = 230.0f;
	static float initialSpacingColumn1 = 80.0f;
	static float initialSpacingColumn2 = 90.0f;
	static ImGuiFileDialogSortOrder fileNameSortOrder = ImGuiFileDialogSortOrder_None;
	static ImGuiFileDialogSortOrder sizeSortOrder = ImGuiFileDialogSortOrder_None;
	static ImGuiFileDialogSortOrder dateSortOrder = ImGuiFileDialogSortOrder_None;
	static ImGuiFileDialogSortOrder typeSortOrder = ImGuiFileDialogSortOrder_None;

	assert(dialogInfo != nullptr);

	bool complete = false;

	ImGui::PushID(dialogInfo);
	ImGui::SetNextWindowSize(ImVec2(740.0f, 410.0f), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(dialogInfo->title.c_str(), open))
	{
		if (dialogInfo->currentFiles.empty() && dialogInfo->currentDirectories.empty() || dialogInfo->refreshInfo)
			RefreshInfo(dialogInfo);

		// Draw path
		ImGui::Text("Path: %s", dialogInfo->directoryPath.string().c_str());

		ImGui::BeginChild("##browser", ImVec2(ImGui::GetWindowContentRegionMax().x, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Columns(4);

		// Columns size
		if (initialSpacingColumn0 > 0)
		{
			ImGui::SetColumnWidth(0, initialSpacingColumn0);
			initialSpacingColumn0 = 0.0f;
		}
		if (initialSpacingColumn1 > 0)
		{
			ImGui::SetColumnWidth(1, initialSpacingColumn1);
			initialSpacingColumn1 = 0.0f;
		}
		if (initialSpacingColumn2 > 0)
		{
			ImGui::SetColumnWidth(2, initialSpacingColumn2);
			initialSpacingColumn2 = 0.0f;
		}

		// File Columns
		if (ImGui::Selectable("Name"))
		{
			sizeSortOrder = ImGuiFileDialogSortOrder_None;
			dateSortOrder = ImGuiFileDialogSortOrder_None;
			typeSortOrder = ImGuiFileDialogSortOrder_None;
			fileNameSortOrder = fileNameSortOrder == ImGuiFileDialogSortOrder_Down ? ImGuiFileDialogSortOrder_Up : ImGuiFileDialogSortOrder_Down;
		}
		ImGui::NextColumn();
		if (ImGui::Selectable("Size"))
		{
			fileNameSortOrder = ImGuiFileDialogSortOrder_None;
			dateSortOrder = ImGuiFileDialogSortOrder_None;
			typeSortOrder = ImGuiFileDialogSortOrder_None;
			sizeSortOrder = sizeSortOrder == ImGuiFileDialogSortOrder_Down ? ImGuiFileDialogSortOrder_Up : ImGuiFileDialogSortOrder_Down;
		}
		ImGui::NextColumn();
		if (ImGui::Selectable("Type"))
		{
			fileNameSortOrder = ImGuiFileDialogSortOrder_None;
			dateSortOrder = ImGuiFileDialogSortOrder_None;
			sizeSortOrder = ImGuiFileDialogSortOrder_None;
			typeSortOrder = typeSortOrder == ImGuiFileDialogSortOrder_Down ? ImGuiFileDialogSortOrder_Up : ImGuiFileDialogSortOrder_Down;
		}
		ImGui::NextColumn();
		if (ImGui::Selectable("Date"))
		{
			fileNameSortOrder = ImGuiFileDialogSortOrder_None;
			sizeSortOrder = ImGuiFileDialogSortOrder_None;
			typeSortOrder = ImGuiFileDialogSortOrder_None;
			dateSortOrder = dateSortOrder == ImGuiFileDialogSortOrder_Down ? ImGuiFileDialogSortOrder_Up : ImGuiFileDialogSortOrder_Down;
		}
		ImGui::NextColumn();

		// File Separator
		ImGui::Separator();

		// Sort directories
		auto *directories = &dialogInfo->currentDirectories;

		if (fileNameSortOrder != ImGuiFileDialogSortOrder_None || sizeSortOrder != ImGuiFileDialogSortOrder_None || typeSortOrder != ImGuiFileDialogSortOrder_None)
		{
			std::sort(directories->begin(), directories->end(), [](const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b)
								{
				if (fileNameSortOrder == ImGuiFileDialogSortOrder_Down)
				{
					return a.path().filename() > b.path().filename();
				}

				return a.path().filename() < b.path().filename(); });
		}
		else if (dateSortOrder != ImGuiFileDialogSortOrder_None)
		{
			std::sort(directories->begin(), directories->end(), [](const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b)
								{
				if (dateSortOrder == ImGuiFileDialogSortOrder_Down)
				{
					return a.last_write_time() > b.last_write_time();
				}
				return a.last_write_time() < b.last_write_time(); });
		}

		// Sort files
		auto *files = &dialogInfo->currentFiles;

		if (fileNameSortOrder != ImGuiFileDialogSortOrder_None)
		{
			std::sort(files->begin(), files->end(), [](const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b)
								{
				if (fileNameSortOrder == ImGuiFileDialogSortOrder_Down)
				{
					return a.path().filename() > b.path().filename();
				}

				return a.path().filename() < b.path().filename(); });
		}
		else if (sizeSortOrder != ImGuiFileDialogSortOrder_None)
		{
			std::sort(files->begin(), files->end(), [](const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b)
								{
				if (sizeSortOrder == ImGuiFileDialogSortOrder_Down)
				{
					return a.file_size() > b.file_size();
				}
				return a.file_size() < b.file_size(); });
		}
		else if (typeSortOrder != ImGuiFileDialogSortOrder_None)
		{
			std::sort(files->begin(), files->end(), [](const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b)
								{
				if (typeSortOrder == ImGuiFileDialogSortOrder_Down)
				{
					return a.path().extension() > b.path().extension();
				}

				return a.path().extension() < b.path().extension(); });
		}
		else if (dateSortOrder != ImGuiFileDialogSortOrder_None)
		{
			std::sort(files->begin(), files->end(), [](const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b)
								{
				if (dateSortOrder == ImGuiFileDialogSortOrder_Down)
				{
					return a.last_write_time() > b.last_write_time();
				}
				return a.last_write_time() < b.last_write_time(); });
		}

		size_t index = 0;

		// Draw parent
		if (dialogInfo->directoryPath.has_parent_path())
		{
			if (ImGui::Selectable("..", dialogInfo->currentIndex == index, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionMax().x, 0)))
			{
				dialogInfo->currentIndex = index;

				if (ImGui::IsMouseDoubleClicked(0))
				{
					dialogInfo->directoryPath = dialogInfo->directoryPath.parent_path();
					dialogInfo->refreshInfo = true;
				}
			}
			ImGui::NextColumn();
			ImGui::TextUnformatted("-");
			ImGui::NextColumn();
			ImGui::TextUnformatted("<parent>");
			ImGui::NextColumn();
			ImGui::TextUnformatted("-");
			ImGui::NextColumn();

			index++;
		}

		// Draw directories
		for (size_t i = 0; i < directories->size(); ++i)
		{
			auto directoryEntry = dialogInfo->currentDirectories[i];
			auto directoryPath = directoryEntry.path();
			auto directoryName = directoryPath.filename();

			if (ImGui::Selectable(directoryName.string().c_str(), dialogInfo->currentIndex == index, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionMax().x, 0)))
			{
				dialogInfo->currentIndex = index;

				if (ImGui::IsMouseDoubleClicked(0))
				{
					dialogInfo->directoryPath = directoryPath;
					dialogInfo->refreshInfo = true;
				}
			}

			ImGui::NextColumn();
			ImGui::TextUnformatted("-");
			ImGui::NextColumn();
			ImGui::TextUnformatted("<directory>");
			ImGui::NextColumn();

			auto lastWriteTime = directoryEntry.last_write_time();
			auto st = std::chrono::time_point_cast<std::chrono::system_clock::duration>(lastWriteTime - decltype(lastWriteTime)::clock::now() + std::chrono::system_clock::now());
			std::time_t tt = std::chrono::system_clock::to_time_t(st);
			std::tm mt;
			localtime_r(&tt, &mt);
			std::stringstream ss;
			ss << std::put_time(&mt, "%F %R");
			ImGui::TextUnformatted(ss.str().c_str());
			ImGui::NextColumn();

			index++;
		}

		// Draw files
		for (size_t i = 0; i < files->size(); ++i)
		{
			auto fileEntry = dialogInfo->currentFiles[i];
			auto filePath = fileEntry.path();
			auto fileName = filePath.filename();

			if (ImGui::Selectable(fileName.string().c_str(), dialogInfo->currentIndex == index, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionMax().x, 0)))
			{
				dialogInfo->currentIndex = index;
				dialogInfo->fileName = fileName;
			}

			ImGui::NextColumn();
			ImGui::TextUnformatted(std::to_string(fileEntry.file_size()).c_str());
			ImGui::NextColumn();
			ImGui::TextUnformatted(filePath.extension().string().c_str());
			ImGui::NextColumn();

			auto lastWriteTime = fileEntry.last_write_time();
			auto st = std::chrono::time_point_cast<std::chrono::system_clock::duration>(lastWriteTime - decltype(lastWriteTime)::clock::now() + std::chrono::system_clock::now());
			std::time_t tt = std::chrono::system_clock::to_time_t(st);
			std::tm mt;
			localtime_r(&tt, &mt);
			std::stringstream ss;
			ss << std::put_time(&mt, "%F %R");
			ImGui::TextUnformatted(ss.str().c_str());
			ImGui::NextColumn();

			index++;
		}
		ImGui::EndChild();

		// Draw filename
		static const size_t fileNameBufferSize = 200;
		static char fileNameBuffer[fileNameBufferSize];

		std::string fileNameStr = dialogInfo->fileName.string();
		size_t fileNameSize = fileNameStr.size();

		if (fileNameSize >= fileNameBufferSize)
			fileNameSize = fileNameBufferSize - 1;
		std::memcpy(fileNameBuffer, fileNameStr.c_str(), fileNameSize);
		fileNameBuffer[fileNameSize] = 0;

		ImGui::PushItemWidth(ImGui::GetWindowContentRegionMax().x);
		if (ImGui::InputText("File Name", fileNameBuffer, fileNameBufferSize))
		{
			dialogInfo->fileName = std::string(fileNameBuffer);
			dialogInfo->currentIndex = 0;
		}

		if (ImGui::Button("Cancel"))
		{
			fileNameSortOrder = ImGuiFileDialogSortOrder_None;
			sizeSortOrder = ImGuiFileDialogSortOrder_None;
			typeSortOrder = ImGuiFileDialogSortOrder_None;
			dateSortOrder = ImGuiFileDialogSortOrder_None;

			dialogInfo->refreshInfo = false;
			dialogInfo->currentIndex = 0;
			dialogInfo->currentFiles.clear();
			dialogInfo->currentDirectories.clear();

			*open = false;
		}

		ImGui::SameLine();

		if (dialogInfo->type == ImGuiFileDialogType_ReadFile)
		{
			if (ImGui::Button("Ok"))
			{
				dialogInfo->resultPath = dialogInfo->directoryPath / dialogInfo->fileName;

				if (std::filesystem::exists(dialogInfo->resultPath))
				{
					fileNameSortOrder = ImGuiFileDialogSortOrder_None;
					sizeSortOrder = ImGuiFileDialogSortOrder_None;
					typeSortOrder = ImGuiFileDialogSortOrder_None;
					dateSortOrder = ImGuiFileDialogSortOrder_None;

					dialogInfo->refreshInfo = false;
					dialogInfo->currentIndex = 0;
					dialogInfo->currentFiles.clear();
					dialogInfo->currentDirectories.clear();

					complete = true;
					*open = false;
				}
			}
		} else if (dialogInfo->type == ImGuiFileDialogType_WriteFile)
		{
			if (ImGui::Button("Ok"))
			{
				dialogInfo->resultPath = dialogInfo->directoryPath / dialogInfo->fileName;
				if (std::filesystem::exists(dialogInfo->resultPath))
				{
					// Popup to check if wish to override file
					ImGui::OpenPopup("Override File Popup");
				} else
				{
					fileNameSortOrder = ImGuiFileDialogSortOrder_None;
					sizeSortOrder = ImGuiFileDialogSortOrder_None;
					typeSortOrder = ImGuiFileDialogSortOrder_None;
					dateSortOrder = ImGuiFileDialogSortOrder_None;

					dialogInfo->refreshInfo = false;
					dialogInfo->currentIndex = 0;
					dialogInfo->currentFiles.clear();
					dialogInfo->currentDirectories.clear();

					complete = true;
					*open = false;
				}
			}
		}
	}

	if (ImGui::BeginPopupModal("Override File Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		
		ImGui::PushStyleColor(ImGuiCol_Text, ImColor(84, 84, 84).Value);
		ImGui::Text("Do you wish to override this file?");
		ImGui::PopStyleColor();
		
		float modalWidth = ImGui::GetContentRegionAvail().x;
		float buttonWidth = (modalWidth - ImGui::GetStyle().ItemSpacing.x) / 2;
		
		if (ImGui::Button("YES", ImVec2(buttonWidth, 0))) {
			fileNameSortOrder = ImGuiFileDialogSortOrder_None;
			sizeSortOrder = ImGuiFileDialogSortOrder_None;
			typeSortOrder = ImGuiFileDialogSortOrder_None;
			dateSortOrder = ImGuiFileDialogSortOrder_None;

			dialogInfo->refreshInfo = false;
			dialogInfo->currentIndex = 0;
			dialogInfo->currentFiles.clear();
			dialogInfo->currentDirectories.clear();

			complete = true;
			*open = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("NO", ImVec2(buttonWidth, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
	ImGui::PopID();

	return complete;
}