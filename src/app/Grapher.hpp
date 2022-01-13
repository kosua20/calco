#pragma once
#include "core/Common.hpp"
#include "core/Calculator.hpp"

#include <imgui/imgui.h>
#include <implot/implot.h>

#undef DOMAIN

struct FunctionGraph {

	enum class Type {
		FUNCTION, DOMAIN
	};

	std::vector<double> values;
	std::vector<Value> args;
	std::vector<glm::vec2> argsRanges;
	std::string name;
	ImVec4 color = ImVec4(1,0,0,1);
	Type type = Type::FUNCTION;
	size_t valuesCount = 0;
	bool show = false;
	bool dirty = true;
	bool invalid = false;
	bool showArgsPanel = false;

	void validate(Calculator& calculator);
	
};

class Grapher {

public:

	FunctionGraph& addOrUpdateFunction(const std::string& name, const Documentation::Function& func);

	void clear();
	
	bool display(Calculator& calculator);

private:

	std::vector<double> _xs;
	std::vector<double> _ys;
	std::vector<FunctionGraph> _functions;
	ImPlotRect _currentRect = ImPlotRect(0, 1, 0, 1);
	int _totalCount = 0;
	int _sampleCount = 100;
	bool _updateRect = true;
	bool _hideInvalids = false;
	bool _hideHiddens = false;

};
