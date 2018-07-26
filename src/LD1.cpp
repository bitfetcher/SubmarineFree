#include "DS.hpp"

template <int x>
struct LD_1 : DS_Module {
	enum ParamIds {
		PARAM_CUTOFF_1,
		PARAM_WIDTH_1 = x,
		NUM_PARAMS = x + x
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS = x
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};

	DS_Schmitt schmittState[x];

	LD_1() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
		for (int i = 0; i < x; i++) {
			outputs[OUTPUT_1 + i].value = output(schmittState[i].state(params[PARAM_CUTOFF_1 + i].value - params[PARAM_WIDTH_1 + i].value, params[PARAM_CUTOFF_1 + i].value + params[PARAM_WIDTH_1 + i].value, inputs[INPUT_1 + i].value));
		}
	}
};

struct LD106 : ModuleWidget {
	static const int deviceCount = 6;	
	ParamWidget *cutoffWidgets[deviceCount];
	ParamWidget *widthWidgets[deviceCount];
	LD106(LD_1<deviceCount> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/LD-106.svg")));

		for (int i = 0; i < deviceCount; i++) {
			int offset = 58 * i;
			addInput(Port::create<SilverPort>(Vec(4,19 + offset), Port::INPUT, module, LD_1<6>::INPUT_1 + i));

			addOutput(Port::create<BluePort>(Vec(62,19 + offset), Port::OUTPUT, module, LD_1<6>::OUTPUT_1 + i));

			cutoffWidgets[i] = ParamWidget::create<SmallKnob<LightKnob>>(Vec(4, 47 + offset), module, LD_1<6>::PARAM_CUTOFF_1 + i, -10.0f, 10.0f, 5.0f);
			addParam(cutoffWidgets[i]);
			widthWidgets[i] = ParamWidget::create<SmallKnob<LightKnob>>(Vec(62, 47 + offset), module, LD_1<6>::PARAM_WIDTH_1 + i, 0.0f, 5.0f, 1.0f);
			addParam(widthWidgets[i]);
		}
	}
	void appendContextMenu(Menu *menu) override;
};

struct LDMenuItem: MenuItem {
	LD106 *ld106;
	float cutoff;
	float width;
	void onAction(EventAction &e) override {
		for (int i = 0; i < LD106::deviceCount; i++) {
			ld106->cutoffWidgets[i]->setValue(cutoff);
			ld106->widthWidgets[i]->setValue(width);
		}
	}
};

void LD106::appendContextMenu(Menu *menu) {
	menu->addChild(MenuEntry::create());
	LD106 *ld106 = dynamic_cast<LD106*>(this);
	assert(ld106);
	LDMenuItem *menuItem = MenuItem::create<LDMenuItem>("Cutoff 5V");
	menuItem->ld106 = ld106;
	menuItem->cutoff = 5.0f;
	menuItem->width = 1.0f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem>("Cutoff 0V");
	menuItem->ld106 = ld106;
	menuItem->cutoff = 0.0f;
	menuItem->width = 0.0f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem>("Cutoff 2.5V");
	menuItem->ld106 = ld106;
	menuItem->cutoff = 2.5f;
	menuItem->width = 0.5f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem>("TTL Levels");
	menuItem->ld106 = ld106;
	menuItem->cutoff = 1.4f;
	menuItem->width = 0.6f;
	menu->addChild(menuItem);
	((DS_Module *)module)->appendContextMenu(menu);
}

Model *modelLD106 = Model::create<LD_1<6>, LD106>("SubmarineFree", "LD-106", "LD-106 Schmitt Trigger Line Drivers", LOGIC_TAG, MULTIPLE_TAG);
