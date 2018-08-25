"Resource/UI/main_menu/LoadoutPanel.res"
{		
	"CTFLoadoutPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFLoadoutPanel"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"f0"
		"tall"				"f0"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"border"			""
	}	

	"BackgroundImage"
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"BackgroundImage"
		"xpos"				"0"
		"ypos"				"20"
		"zpos"				"-2"
		"wide"				"f0"
		"tall"				"f+40"
		"autoResize"		"0"
		"pinCorner"			"0"
		"scaleimage"		"1"
		"border"			"AdvRoundedButtonDefault"
		"visible"			"1"
		"fillcolor"			"46 53 42 255"
		"enabled"			"1"
	}

	"ShadedBackgroundImage"
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"ShadedBackgroundImage"
		"xpos"				"0"
		"ypos"				"21"
		"zpos"				"-1"
		"wide"				"f0"
		"tall"				"60"
		"autoResize"		"0"
		"pinCorner"			"0"
		"scaleimage"		"1"
		"image"				"gradient_pure_black"
		"visible"			"1"
		"enabled"			"1"
	}

	"BackgroundHeader"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"BackgroundHeader"
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"-2"
		"wide"			"f0"
		"tall"			"80"
		"visible"		"1"
		"enabled"		"1"
		"image"			"loadout_header"
		"tileImage"		"1"
	}	
	"BackgroundHeaderBP"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"BackgroundHeaderBP"
		"xpos"			"115"
		"ypos"			"5"
		"zpos"			"-1"
		"wide"			"620"
		"tall"			"75"
		"visible"		"1"
		"enabled"		"1"
		"border"		"ReplayOutlinedGreyBox"
		"tileImage"		"1"
	}				
	"BackgroundFooter"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"BackgroundFooter"
		"xpos"			"0"
		"ypos"			"420"
		"zpos"			"1"
		"wide"			"f0"
		"tall"			"60"
		"visible"		"1"
		"enabled"		"1"
		"image"			"loadout_bottom_gradient"
		"tileImage"		"1"
	}
	"FooterLine"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"FooterLine"
		"xpos"			"0"
		"ypos"			"420"
		"zpos"			"2"
		"wide"			"f0"
		"tall"			"10"
		"visible"		"1"
		"enabled"		"1"
		"image"			"loadout_solid_line"
		"scaleImage"	"1"
	}
	"HeaderLine"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"HeaderLine"
		"xpos"			"0"
		"ypos"			"80"
		"zpos"			"5"
		"wide"			"f0"
		"tall"			"10"
		"visible"		"1"
		"enabled"		"1"
		"image"			"loadout_solid_line"
		"scaleImage"	"1"
	}				

	"ShadedBarBottom"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"ShadedBarBottom"
		"xpos"			"0"
		"ypos"			"365"
		"zpos"			"-1"
		"wide"			"f0"
		"tall"			"55"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"	
		"fillcolor"		"0 0 0 153"
		"PaintBackgroundType"	"0"
	}

	"ShadedBarTop"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"ShadedBarTop"
		//"xpos"			"0"
		//"ypos"			"21"
		//"zpos"			"-1"
		"xpos"			"0"
		"ypos"			"80"
		"zpos"			"-1"
		"wide"			"f0"
		"tall"			"55"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"	
		"fillcolor"		"0 0 0 153"
		"PaintBackgroundType"	"0"
	}
	
	"classmodelpanel"
	{
		"ControlName"	"CTFAdvModelPanel"
		"fieldName"		"classmodelpanel"
		"xpos"			"c-360"
		"ypos"			"c-115"
		"zpos"			"0"		
		"wide"			"280"
		"tall"			"300"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"

		"fov"			"25"
		"allow_rot"		"1"
		//"allow_pitch"	"1"
		"allow_manip"	"1"
				
		"model"
		{
			"force_pos"	"1"
			"skin"	"0"

			"angles_x" "0"
			"angles_y" "210"
			"angles_z" "0"
			"origin_x" "190"
			"origin_y" "0"
			"origin_z" "-40"
			"frame_origin_x"	"0"
			"frame_origin_y"	"0"
			"frame_origin_z"	"0"
			"spotlight" "1"
		
			"modelname"		"models/player/heavy.mdl"
			"modelname_hwm"	"models/player/hwm/heavy.mdl"
			
			"attached_model"
			{
				"modelname" "models/weapons/w_models/w_flamethrower.mdl"
				"skin"	"0"
			}
			
			"animation"
			{
				"name"		"PRIMARY"
				"activity"	"ACT_MP_STAND_PRIMARY"
				"default"	"1"
			}
			"animation"
			{
				"name"		"SECONDARY"
				"activity"	"ACT_MP_STAND_SECONDARY"
			}
			"animation"
			{
				"name"		"MELEE"
				"activity"	"ACT_MP_STAND_MELEE"
			}
			"animation"
			{
				"name"		"GRENADE"
				"activity"	""
			}	
			"animation"
			{
				"name"		"BUILDING"
				"activity"	"ACT_MP_STAND_BUILDING"
			}	
			"animation"
			{
				"name"		"PDA"
				"activity"	"ACT_MP_STAND_PDA"
			}	
			"animation"
			{
				"name"		"ITEM1"
				"activity"	"ACT_MP_STAND_ITEM1"
			}
			"animation"
			{
				"name"		"ITEM2"
				"activity"	"ACT_MP_STAND_ITEM2"
			}
			"animation"
			{
				"name"		"MELEE_ALLCLASS"
				"activity"	"ACT_MP_STAND_MELEE_ALLCLASS"
			}
			"animation"
			{
				"name"		"SECONDARY2"
				"activity"	"ACT_MP_STAND_SECONDARY2"
			}
			"animation"
			{
				"name"		"PRIMARY2"
				"activity"	"ACT_MP_STAND_PRIMARY2"
			}
		}
	}

	"weaponsetpanel"
	{
		"ControlName"	"CTFWeaponSetPanel"
		"fieldName"		"weaponsetpanel"		
		"xpos"			"c-70"
		"ypos"			"c-100"
		"zpos"			"2"		
		"wide"			"308"
		"tall"			"280"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		//"border"		"TFFatLineBorder"
	}

	"rgbpanel"
	{
		"ControlName"	"CTFRGBPanel"
		"fieldName"		"rgbpanel"		
		"xpos"			"c-80"
		"ypos"			"c-70"
		"zpos"			"-1"		
		"wide"			"230"
		"tall"			"280"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
		"border"		"TFFatLineBorder"
	}
	
	"BackButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"BackButton"
		"xpos"			"c-295"
		"ypos"			"437"
		"zpos"			"2"
		"wide"			"124"
		"tall"			"25"
		"autoResize"	"0"
		"pinCorner"		"3"
		"visible"		"1"
		"enabled"		"1"
		"command"		"back"		
		
		"SubButton"
		{
			"labelText" 		"<< Back (&Esc)"
			"xshift" 			"0"
			"yshift" 			"-2"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
	}	

	"ResetButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"ResetButton"
		"xpos"			"c150"
		"ypos"			"437"
		"zpos"			"2"
		"wide"			"124"
		"tall"			"25"
		"visible"		"1"
		"enabled"		"1"
		"command"		"resetloadout"		
		
		"SubButton"
		{
			"labelText" 		"Reset"
			"xshift" 			"0"
			"yshift" 			"-2"
			"textAlignment"		"center"
			"font"				"HudFontSmallBold"
			"tooltip"			"Reset All Selected Weapons"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
	}	

	"CaratLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"CaratLabel"
		"font"			"HudFontSmallestBold"
		"labelText"		">>"
		"textAlignment"	"west"
		"xpos"			"c-300"
		"ypos"			"105"
		"zpos"			"1"
		"wide"			"20"
		"tall"			"15"
		"autoResize"	"1"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"fgcolor_override" "200 80 60 255"
	}
	"ClassLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ClassLabel"
		"xpos"				"c-280"
		"ypos"				"95"
		"zpos"				"5"
		"wide"				"500"
		"tall"				"30"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%classname%"
		"textAlignment"		"west"
		"font"				"TeamMenuBold"
		"fgcolor"			"Button.ArmedTextColor"
	}	
	
	"ClassLabelShadow"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ClassLabelShadow"
		"xpos"				"c-278"
		"ypos"				"100"
		"zpos"				"4"
		"wide"				"500"
		"tall"				"30"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%classname%"
		"textAlignment"		"west"
		"font"				"TeamMenuBold"
		"fgcolor"			"Black"
	}	

	"classselection"
	{
		"ControlName"		"CAdvTabs"
		"fieldName"			"classselection"
		"xpos"				"c-300"
		"ypos"				"10"
		"zpos"				"6"
		"wide"				"600"
		"tall"				"63"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"		
		"offset"			"3"
	
	
		"scout_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"scout_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"		
			"command"			"select_scout"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/scout"	
				"imagewidth"		"55"
			}
		}
	
		"soldier_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"soldier_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"		
			"command"			"select_soldier"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/soldier"	
				"imagewidth"		"55"
			}
		}
	
		"pyro_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"pyro_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_pyro"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/pyro"	
				"imagewidth"		"55"
			}
		}
	
		"demoman_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"demoman_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_demoman"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/demo"	
				"imagewidth"		"55"
			}
		}
	
		"heavyweapons_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"heavyweapons_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_heavyweapons"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/heavy"	
				"imagewidth"		"55"
			}
		}
	
		"engineer_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"engineer_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_engineer"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/engineer"	
				"imagewidth"		"55"
			}
		}
	
		"medic_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"medic_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_medic"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/medic"	
				"imagewidth"		"55"
			}
		}
	
		"sniper_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"sniper_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_sniper"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/sniper"	
				"imagewidth"		"55"
			}
		}
	
		"spy_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"spy_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_spy"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
				"depressedFgColor_override"		"HudProgressBarInActive"
				"armedFgColor_override"			"HudTrainingHint"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/spy"	
				"imagewidth"		"55"
			}
		}
	}	
}
