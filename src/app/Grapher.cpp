#include "Grapher.hpp"


void FunctionGraph::validate(Calculator& calculator){
   // Test the validity of the function.
   Value outRaw, outFloat;
   invalid = true;
   if(calculator.evaluateFunction(name, args, outRaw)){
	   if(outRaw.type == Value::Type::BOOL){
		   type = Type::DOMAIN;
		   invalid = false;
	   } else if(outRaw.convert(Value::Type::FLOAT, outFloat)){
		   type = Type::FUNCTION;
		   invalid = false;
	   }
	   // Else conversion failure.
   }
}

FunctionGraph& Grapher::addOrUpdateFunction(const std::string& name, const Documentation::Function& func){
	// Did the function already exist?
	int existingID = -1;
	const size_t funcCount = _functions.size();
	for(size_t fid = 0; fid < funcCount; ++fid){
		if(_functions[fid].name == name){
			existingID = int(fid);
			break;
		}
	}
	if(existingID == -1){
		existingID = int(funcCount);
		_functions.emplace_back();
	}

	// Retrieve or create function graph infos.
	FunctionGraph& funcGraph = _functions[existingID];
	funcGraph.name = name;
	funcGraph.values.clear();
	// Reset arguments and their ranges.
	const size_t argCount = func.arguments.size();
	funcGraph.args.resize(argCount);
	funcGraph.argsRanges.resize(argCount);
	for(size_t aid = 0; aid < argCount; ++aid){
		funcGraph.args[aid] = 0.0f;
		funcGraph.argsRanges[aid] = glm::vec2(0.0f, 1.0f);
	}

	// If existing function graph, preserve set color, else revert.
	if(existingID == int(funcCount)){
		funcGraph.color = ImPlot::GetColormapColor(_totalCount++);
	}

	funcGraph.show = false;
	funcGraph.dirty = true;
	funcGraph.invalid = false;
	funcGraph.showArgsPanel = false;
	funcGraph.valuesCount = 0;
	return funcGraph;
}

void Grapher::clear(){
	_functions.clear();
	_totalCount = 0;
}

bool Grapher::display(Calculator& calculator){
	bool refresh = false;

	// Left
	{
		const float panelWidth = 340.0f;
		ImGui::BeginGroup();

		ImGui::Text("Show:");
		ImGui::SameLine();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0.5));
		if (ImGui::Button("All")) {
			for (auto& graph : _functions) {
				graph.show = !graph.invalid;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("None")) {
			for (auto& graph : _functions) {
				graph.show = false;
			}
		}
		ImGui::SameLine();
		ImGui::Text("Hide:");
		ImGui::SameLine();
		ImGui::Checkbox("Errors", &_hideInvalids);
		ImGui::SameLine();
		ImGui::Checkbox("Disabled", &_hideHiddens);
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 8.0f);
		ImGui::BeginChild("##Grapher Left panel", ImVec2(panelWidth, 0), true);

		for(auto& graph : _functions) {
			const Documentation::Function& ref = calculator.functions().at(graph.name);
			const size_t argCount = graph.args.size();
			// Skip invalid if requested.
			if (_hideInvalids && graph.invalid) {
				continue;
			}
			if(_hideHiddens && !graph.show){
				continue;
			}

			ImGui::PushID(graph.name.c_str());

			ImGui::BeginDisabled(graph.invalid);

			// Color selector
			ImVec4 color = graph.invalid ? ImVec4(0.3f, 0.3f, 0.3f, 1.0f) : (graph.show ? graph.color : ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0.5));
			ImGui::ColorEdit3("##color", graph.invalid ? &color.x : &graph.color.x, ImGuiColorEditFlags_NoInputs);
			ImGui::PopStyleVar();

			ImGui::SameLine();

			// Then the name, arguments and expression of the function.
			ImGui::PushStyleColor(ImGuiCol_Text, color);
			const float wrapPos = panelWidth - ImGui::GetFrameHeightWithSpacing();
			ImGui::PushTextWrapPos(wrapPos);
			// Selectable row to show/hide
			if (ImGui::Selectable("##id", false, ImGuiSelectableFlags_AllowItemOverlap)) {
				graph.show = !graph.show;
			}
			ImGui::SameLine(0, 0);
			ImGui::TextWrapped("%s = %s", ref.name.c_str(), ref.expression.c_str());
			ImGui::PopTextWrapPos();
			ImGui::PopStyleColor();

			const size_t firstFixedArg = graph.type == FunctionGraph::Type::DOMAIN ? 2 : 1;

			if(argCount > firstFixedArg){

				ImGui::SameLine(wrapPos, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0.5));
				if(ImGui::ArrowButton("##valuesButton", graph.showArgsPanel ? ImGuiDir_Up : ImGuiDir_Down)){
					graph.showArgsPanel = !graph.showArgsPanel;
				}
				ImGui::PopStyleVar();

				if (graph.showArgsPanel) {
					// Draw sliders for each extra argument.
					for (size_t aid = firstFixedArg; aid < argCount; ++aid) {
						ImGui::PushID(ref.arguments[aid].c_str());

						float f = float(graph.args[aid].f);
						glm::vec2& range = graph.argsRanges[aid];

						ImGui::AlignTextToFramePadding();
						ImGui::TextUnformatted(ref.arguments[aid].c_str());
						ImGui::SameLine(50);

						ImGui::PushItemWidth(50);
						if (ImGui::DragFloat("##Min", &range.x, 1.f, -FLT_MAX, range.y, "%.2f", 0)) {
							if (range.x > range.y) {
								std::swap(range.x, range.y);
							}
						}

						ImGui::PushItemWidth(150);
						ImGui::SameLine();
						if (ImGui::SliderFloat("##slider", &f, range.x, range.y)) {
							graph.dirty = true;
							graph.args[aid].f = double(f);
						}
						ImGui::PopItemWidth();
						ImGui::SameLine();

						if (ImGui::DragFloat("##Max", &range.y, 1.f, range.x, FLT_MAX, "%.2f", 0)) {
							if (range.x > range.y) {
								std::swap(range.x, range.y);
							}
						}
						ImGui::PopItemWidth();

						ImGui::PopID();

					}
				}

			}

			ImGui::EndDisabled();
			ImGui::Separator();
			ImGui::PopID();

		}
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::EndGroup();
	}


	// Right
	ImGui::SameLine();
	{
		ImGui::BeginGroup();

		// Display options.
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0.5));
		if(ImGui::Button("Center")){
			const double minX = _currentRect.X.Min;
			const double maxX = _currentRect.X.Max;
			const double minY = _currentRect.Y.Min;
			const double maxY = _currentRect.Y.Max;
			const double newX = glm::max(abs(minX), abs(maxX));
			const double newY = glm::max(abs(minY), abs(maxY));
			ImPlot::SetNextAxesLimits(-newX, newX, -newY, newY, ImPlotCond_Always);
		}
		ImGui::SameLine();
		if(ImGui::Button("Unit")){
			ImPlot::SetNextAxesLimits(-1.0, 1.0, -1.0, 1.0, ImPlotCond_Always);
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		if(ImGui::SliderInt("Sampling", &_sampleCount, 50, 2048)){
			_updateRect = true;
			_sampleCount = glm::max(50, _sampleCount);

		}
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		// Update sampled data as late as possible.

		// If the graph region was resized, update the abscisse samples.
		if(_updateRect){
			_xs.resize(_sampleCount);

			const float aspectRatio =  float(_currentRect.Y.Max - _currentRect.Y.Min) / float(_currentRect.X.Max - _currentRect.X.Min);

			const size_t ySampleCount = size_t(glm::floor(float(_sampleCount) * aspectRatio));
			_ys.resize(ySampleCount);

			for(int i = 0; i < _sampleCount; ++i){
				_xs[i] = (double(i)+0.5)/ double(_sampleCount) * (_currentRect.X.Max - _currentRect.X.Min) + _currentRect.X.Min;
			}
			for(size_t i = 0; i < ySampleCount; ++i){
				_ys[i] = (double(i)+0.5)/ double(ySampleCount) * (_currentRect.Y.Max - _currentRect.Y.Min) + _currentRect.Y.Min;
			}
			_updateRect = false;
			// Mark all graphs as dirty.
			for(auto& graph : _functions){
				graph.dirty = true;
			}
		}
		 // Update functions.
		for(auto& graph : _functions){
			if(!graph.show){
				continue;
			}
			if(!graph.dirty){
				continue;
			}

			if(graph.type == FunctionGraph::Type::FUNCTION){
				// Sample linearly for abscisse values.
				graph.values.resize(_sampleCount);
				graph.valuesCount = _sampleCount;

				for(int sid = 0; sid < _sampleCount; ++sid){
					// Set the value of the first argument.
					if(!graph.args.empty()){
						graph.args[0] = _xs[sid];
					}
					// Evaluate the function.
					Value outRaw, outFloat;
					if(!calculator.evaluateFunction(graph.name, graph.args, outRaw)){
						// Evaluation error, hide the function.
						graph.show = false;
						graph.invalid = true;
						break;
					}
					// Convert to float
					if(!outRaw.convert(Value::Type::FLOAT, outFloat)){
						graph.show = false;
						graph.invalid = true;
						break;
					}
					graph.values[sid] = outFloat.f;
				}
			} else if(graph.type == FunctionGraph::Type::DOMAIN){
				const int downscale = 2;
				const size_t sizeX = _xs.size();
				const size_t sizeY = _ys.size();

				graph.values.resize(2 * sizeX/downscale * sizeY/downscale);
				graph.valuesCount = 0;
				const size_t argCount = graph.args.size();

				for(size_t sid = 0; sid < sizeX; sid += downscale){
					// Set the value of the first argument.
					if(argCount != 0){
						graph.args[0] = _xs[sid];
					}

					for(size_t tid = 0; tid < sizeY; tid += downscale){
						if(argCount > 1){
							graph.args[1] = _ys[tid];
						}
						// Evaluate the function.
						Value outRaw;
						if(!calculator.evaluateFunction(graph.name, graph.args, outRaw)){
							// Evaluation error, hide the function.
							graph.show = false;
							graph.invalid = true;
							break;
						}
						assert(outRaw.type == Value::Type::BOOL);
						// Output the point if the test value is positive.
						if(outRaw.b){
							graph.values[2 * graph.valuesCount] = _xs[sid];
							graph.values[2 * graph.valuesCount + 1] = _ys[tid];
							++graph.valuesCount;
						}
					}

					if(graph.invalid){
						break;
					}
				}

			}

			graph.dirty = false;
			// We'll want the display to update.
			refresh = true;
		}

		ImGui::BeginChild("##Function view", ImVec2(0, 0));

		if(ImPlot::BeginPlot("My Plot", ImVec2(-1,-1), ImPlotFlags_NoTitle | ImPlotFlags_NoLegend |  ImPlotFlags_AntiAliased)) {
			ImPlot::SetupAxes(NULL, NULL);
			ImPlot::SetupFinish();
			for(const auto& graph : _functions){
				if(!graph.show){
					continue;
				}

				ImPlot::PushStyleColor(ImPlotCol_Line, graph.color);

				if(graph.type == FunctionGraph::Type::FUNCTION){
					ImPlot::PlotLine(graph.name.c_str(), _xs.data(), graph.values.data(), int(graph.valuesCount));
				} else if(graph.type == FunctionGraph::Type::DOMAIN){
					ImPlot::PlotScatter(graph.name.c_str(), &graph.values[0], &graph.values[1], int(graph.valuesCount), 0, sizeof(double) * 2);
				}
				ImPlot::PopStyleColor();
			}

			ImPlotRect rect = ImPlot::GetPlotLimits();
			if(rect.X.Min != _currentRect.X.Min || rect.X.Max != _currentRect.X.Max ||
			   rect.Y.Min != _currentRect.Y.Min || rect.Y.Max != _currentRect.Y.Max){
				_currentRect = rect;
				_updateRect = true;
			}
			ImPlot::EndPlot();
		}
		ImGui::EndChild();
		ImGui::EndGroup();
	}

	return refresh;
}
