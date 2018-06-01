"Resource/UI/main_menu/CreateServerServerPanel.res"
{
	"CTFCreateMultiplayerGameServerPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFCreateMultiplayerGameServerPanel"
		"xpos"				"c-250"
		"ypos"				"c-200"
		"wide"				"500"
		"tall"				"400"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"border"				""
	}
	
	"ColorLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ColorLabel"
		"xpos"				"10"
		"ypos"				"20"
		"zpos"				"-1"
		"wide"				"600"
		"tall"				"50"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"0"
		"enabled"			"1"
		"labelText"			"#GameUI_MultiplayerAdvanced"
		"textAlignment"		"west"
		"font"				"HudFontBiggerBold"
		"fgcolor"			"AdvTextDefault"
	}	
	
	"PanelListPanel"
	{
		"ControlName"	"CPanelListPanel"
		"fieldName"		"PanelListPanel"
		"xpos"			"20"
		"ypos"			"50"
		"zpos"			"1"
		"wide"			"360"
		"tall"			"310"
		"autoResize"	"0"
		"pinCorner"		"0"
		"font"			"FontStorePromotion"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
	}	
			
	"OK"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"		"OK"
		"xpos"		"750"
		"ypos"		"730"
		"zpos"				"2"
		"wide"		"80"
		"tall"		"50"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"1"
		"bgvisible"		"0"
		"bordervisible"		"0"
		"labelText"		"#GameUI_OK"
		"textAlignment"		"center"
		"dulltext"		"0"
		"command"		"Ok"
		"default"		"1"
	}
	
	"Cancel"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"		"Cancel"
		"xpos"		"850"
		"ypos"		"730"
		"zpos"				"2"
		"wide"		"100"
		"tall"		"50"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"2"
		"bgvisible"		"0"
		"bordervisible"		"0"
		"labelText"		"#GameUI_Cancel"
		"textAlignment"		"center"
		"dulltext"		"0"
		"command"		"vguicancel"
		"default"		"0"
	}
	
}