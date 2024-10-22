/*
Copyright (c) : 2010-2021 ITER Organization, CS 90 046
13067 St. Paul-lez-Durance Cedex
France

This product is part of ITER CODAC software.
For the terms and conditions of redistribution or use of this software
refer to the file ITER-LICENSE.TXT located in the top level directory
of the distribution package.
*/

/*
V2.1 Jul 2018 - support of only one OPI frame (canvas) for integration
INPUT           : macro defining the navigation xml configuration file
LEVEL           : macro defining which is the current level in the navigation
CANVAS_OPI      : macro defining that only one frame is used TRUE/FALSE
MIMIC_FILE      : macro defining which mimics has to be linked to the main canvas
OPI_FILE        : macro defining which OPI to open (button action)
OPI_TYPE        : macro defining if it is a USER opi or the ALARMS LIST
TITLE           : macro defining the title/description of the mimic
TOP_OPI         : macro defining the top ITER.opi or ITERAlarm.opi
CBS             : macro defining the current CBS from CBS1 .. CBS5
CBS_PATH        : macro defining the CBS path: ITER/CBS1/CBS2/CBS3/CBS4/CBS5
*/

importPackage(Packages.org.csstudio.opibuilder.scriptUtil);
importPackage(Packages.java.lang)

    // getting the current cbs level for this screen ITER-CBS1-CBS2-CBS3
    var current_level = (widget.getMacroValue("LEVEL") == null ? "" :
            widget.getMacroValue("LEVEL").toUpperCase());
    var depth = getNavigationDepth(current_level);
    var path = "";

    // getting if a canvas (frame) OPI is used to load mimics or file OPI
    var canvas_opi = (widget.getMacroValue("CANVAS_OPI") == null ? "FALSE" :
            widget.getMacroValue("CANVAS_OPI").toUpperCase());
    if (canvas_opi == "TRUE") {
        opi_macro = "MIMIC_FILE";
    } else {
        canvas_opi = "FALSE";
        opi_macro = "OPI_FILE";
    }

    // getting the type of OPI: USER or ALARMS LIST
    var opi_type = (widget.getMacroValue("OPI_TYPE") == null ? "USER" :
            widget.getMacroValue("OPI_TYPE").toUpperCase());
    if (opi_type != "ALARM") {
        opi_type = "USER";
    }

    // getting the top OPI
    var top_opi = (widget.getMacroValue("TOP_OPI") == null ? "../ITER" :
            widget.getMacroValue("TOP_OPI"));
    if (opi_type == "ALARM") {
        top_opi = "AlarmsList";
    }

    // getting the input xml file path
    var xml_input = (widget.getMacroValue("INPUT") == null ?
            "../navigation/Navigation.xml" :
            widget.getMacroValue("INPUT"));

    // loading XML document and getting the root element
    // the result is a JDOM Element
    var root = FileUtil.loadXMLFile(xml_input, widget);
    if (root) {
        // browsing the CBS tree structure starting from 0 to depth
        listCBS(root, 0, depth);
    }

// ---

// recursive list function on CBS tree
function listCBS(current, iLevel, depth){
    var cbs = current.getChildren();
    if (cbs) {
        var itr = cbs.iterator();
        while (itr.hasNext() && iLevel <= depth) {
            var elt = itr.next();
            if (currentCBS(elt)) {
                updateGlobalNavigationButtons(elt, iLevel);
                // continue the parsing of the CBS navigation tree
                listCBS(elt, iLevel + 1, depth);
                if (lastNavigationLevel(iLevel, depth)) {
                    updateMimicNavigationButtons(current, elt);
                }
            }
        }
    }
}

function getNavigationDepth(current_level) {
    if (current_level == "" || !current_level) {
        // CBS 0 is not specified or empty name
        return 0;
    }
    // getting the number of levels
    // for instance UTIL-S15-AG07 has 3 CBS levels - ITER is CBS0
    var words = current_level.split("-");
    return words.length - 1;
}

function currentCBS(elt) {
    if (getNavigationDepth(current_level) == 0) {
        return true;
    }

    var cbs = elt.getAttributeValue("name").toUpperCase();
    var cbs_path = (path == "" ? cbs : path + "-" + cbs);
    if (cbs_path && current_level.startsWith(cbs_path)) {
        path = cbs_path;
        return true;
     }
     return false;
}

function lastNavigationLevel(currentLevel, lastLevel){
    return (currentLevel == lastLevel)
}

function updateGlobalNavigationButtons(thisCBS, level){
    if (level == 0){
        // adding the Home button for level 0 - ITER overview
        addHomeButton(thisCBS);
    } else if (level <= 5) {
        // adding an Up button for intermediate levels - maximum 5 levels
        addUpButton(thisCBS);
    }
}

function updateMimicNavigationButtons(parentCBS, thisCBS){
    if (thisCBS.getChildren().size()) {
        // adding Mimic buttons
        addMimicButtons(thisCBS);
    } else {
        // repeat parent Mimic buttons
        addMimicButtons(parentCBS);
        disableMimicButton(thisCBS.getAttributeValue("name"));
    }
}

function getGeneralNavigationContainer() {
    return display.getWidget("GENERAL NAVIGATION BAR");
}

function getGeneralNavigationContainerHeight() {
    return getGeneralNavigationContainer().getPropertyValue("height");
}

function getGeneralNavigationContainerWidth() {
    return getGeneralNavigationContainer().getPropertyValue("width");
}

function getMimicNavigationContainer() {
    return display.getWidget("MIMIC SPECIFIC - NAVIGATION AREA");
}

function getMimicNavigationContainerHeight() {
    return getMimicNavigationContainer().getPropertyValue("height");
}

function getMimicNavigationContainerWidth() {
    return getMimicNavigationContainer().getPropertyValue("width");
}

function getLineOneNavigationContainer() {
    return getMimicNavigationContainer().getWidget("OneLineNavigation");
}

function getLineTwoNavigationContainer() {
    return getMimicNavigationContainer().getWidget("TwoLineNavigation");
}

function setNavigationButtonProperties(linkingContainer, elt, cbs_name) {
    // adding opi_file macros to the container
    addOPImacros(linkingContainer, elt);
    // reading attributes from element using JDOM
    linkingContainer.addMacro("CBS", cbs_name);
    linkingContainer.addMacro("CBS_PATH", path.toUpperCase());
    linkingContainer.addMacro("TITLE", getDescription(elt).toUpperCase());
    linkingContainer.addMacro("TOP_OPI", top_opi);
    // update of MIMIC_FILE (if CANVAS_OPI=TRUE) or OPI_FILE with
    // specified user OPI or AlarmsList OPI
    linkingContainer.addMacro(opi_macro, getOPI_FILE(elt));
    linkingContainer.addMacro("ALARM_ROOT", getALARM_ROOT(elt));
    linkingContainer.addMacro("ALARM_FILTER", getALARM_FILTER(elt));
}

function addHomeButton(elt) {
    // if no name defined, ITER is the default cbs name
    var cbs_name = (!elt.getAttributeValue("name") ? "ITER" :
            elt.getAttributeValue("name")).toUpperCase();

    // creating the linking container that displays the HOME button
    var linkingContainer = createHomeButtonContainer();
    if (linkingContainer) {
        setNavigationButtonProperties(linkingContainer, elt, cbs_name);

        // adding the linking container to the navigation widget
        getGeneralNavigationContainer().addChildToRight(linkingContainer);

        // setting the navigation button properties
        var button = getGeneralNavigationContainer().getWidget(cbs_name);
        button.setPropertyValue("height", getGeneralNavigationContainerHeight());
        button.setPropertyValue("width", getGeneralNavigationContainerWidth()/3);
        setButton(button, elt);
    }
}

function createHomeButtonContainer() {
    var height = getGeneralNavigationContainerHeight();
    var width = getGeneralNavigationContainerWidth()/3;
    return createButtonContainer("NavigationHomeButton.opi", width, height);
}

function addUpButton(elt) {
    var cbs_name = elt.getAttributeValue("name").toUpperCase();
    // creating the linking container that displays the Up button
    var linkingContainer = createUpButtonContainer();
    if (linkingContainer) {
        setNavigationButtonProperties(linkingContainer, elt, cbs_name);

        // adding the linking container to the navigation widget
        getGeneralNavigationContainer().addChildToRight(linkingContainer);

        // setting the navigation button properties
        var button = getGeneralNavigationContainer().getWidget(path.toUpperCase());
        button.setPropertyValue("height", getGeneralNavigationContainerHeight());
        button.setPropertyValue("width", (getGeneralNavigationContainerWidth()*2/3)/5);
        setButton(button, elt);
    }
}

function createUpButtonContainer() {
    var height = getGeneralNavigationContainerHeight();
    var width = getGeneralNavigationContainerWidth();
    var home = getGeneralNavigationContainerWidth()/3;
    return createButtonContainer("NavigationUpButton.opi", (width - home)/5, height);
}

function addMimicButtons(root) {
    var cbs = root.getChildren();

    var container = getMimicNavigationContainer();

    var nbButtons = cbs.size();
    var nbButtonsPerLine = 10;

    // default button size for maximum 10 buttons on one line
    var height = getMimicNavigationContainerHeight();
    var width = (getMimicNavigationContainerWidth()/nbButtonsPerLine)-1;

    // if more than 10 buttons required, 2 lines of buttons are needed
    if (nbButtons > nbButtonsPerLine) {
        container = getLineOneNavigationContainer();
        height = height/2;
        // if more than 20 buttons, they have to fit on 2 lines
        if (nbButtons > (2 * nbButtonsPerLine)) {
            nbButtonsPerLine = Math.ceil(nbButtons/2);
            width = getMimicNavigationContainerWidth()/nbButtonsPerLine - 1;
        }
    } else {
        // cleaning the one and two line child containers are they are not
        // needed
        container.removeAllChildren();
    }

    // iterating over all children in XML
    var i = 0;
    var itr = cbs.iterator();
    while (itr.hasNext()) {
        var elt = itr.next();

        if (elt.getAttributeValue("name")) {
            var cbs_name = elt.getAttributeValue("name").toUpperCase();
            // creating the linking container that displays the mimic specific
            // buttons
            var linkingContainer = createButtonContainer("NavigationMimicButton.opi", width, height);

            if (i == nbButtonsPerLine) {
                container = getLineTwoNavigationContainer();
            }

            setNavigationButtonProperties(linkingContainer, elt, cbs_name);

            // adding the linking container to the navigation widget
            container.addChildToRight(linkingContainer);

            // reading value of children in XML using JDOM
            // setting navigation button properties
            var button = container.getWidget(elt.getAttributeValue("name").toUpperCase());
            button.setPropertyValue("height", height);
            button.setPropertyValue("width", width);
            setButton(button, elt);

            i+=1;
         }
    }
}

function disableMimicButton(CBS) {
     var button = getMimicNavigationContainer().getWidget(CBS.toUpperCase());
     button.setPropertyValue("enabled", "false");
}

function getOPI_FILE(elt) {
    var attribute = "";
    if (opi_type == "USER") {
        // getting the opi file from the navigation xml configuration file
        attribute = elt.getAttributeValue("opi_file");
    } else {
        // getting the top opi file from a macro
        attribute = top_opi;
    }

    if (attribute && attribute.search(".opi") > 0) {
        // suppressing the extension .opi and all other macro values
        return attribute.substring(0, attribute.search(".opi"));
    }
    return attribute;
}

function getALARM_ROOT(elt) {
    var alarm_root=elt.getAttributeValue("alarm_root")
    if (alarm_root) {
        return alarm_root;
    } else {
        return widget.getMacroValue("ALARM_ROOT");
    }
}

function getALARM_FILTER(elt) {
    var alarm_filter=elt.getAttributeValue("alarm_filter")
    if (alarm_filter) {
        return alarm_filter;
    } else {
        alarm_filter=""
        var alarm_root=getALARM_ROOT(elt)
        if (alarm_root) {
            var branches = alarm_root.split("/");
            var depth = branches.length;
            if (depth > 1) {
                alarm_filter = branches[depth-1];
            }
        }
        return alarm_filter;
    }
}

function getDescription(elt) {
    return elt.getAttributeValue("description");
}

function addOPImacros(container, elt) {
    // getting the opi file from the navigation xml configuration file (even for
    // alarms list)
    var attribute = elt.getAttributeValue("opi_file");

    if (attribute) {
        var words = attribute.match(/(?:[^\s']+|'[^']*')+/g);

        var i=0;
        for (i in words) {
            if (words[i].search("=") > 0) {
                var macros = words[i].split("=");
                // macro format: macro_name=macro_value
                var macro_name = macros[0];
                var macro_value = (macros[1] == null) ? "" : macros[1];
                if (macro_name == "OPI_FILE" && opi_type != "USER") {
                    macro_value=top_opi;
                }
                container.addMacro(macro_name, macro_value);
            }
        }
    }
}

function createButtonContainer(opi_container, width, height) {
    var linkingContainer = WidgetUtil.createWidgetModel("org.csstudio.opibuilder.widgets.linkingContainer");

    linkingContainer.setPropertyValue("opi_file", opi_container);
    linkingContainer.setPropertyValue("border_style", 0);
    linkingContainer.setPropertyValue("width", width);
    linkingContainer.setPropertyValue("height", height);
    linkingContainer.setPropertyValue("resize_behaviour", 2);

    return linkingContainer;
}

function setButton(button, elt) {
    // reading value of children in XML using JDOM
    // setting navigation button properties
     button.setPropertyValue("tooltip", elt.getAttributeValue("description") + " ($(number_alarms) alarm(s))");
     button.setPropertyValue("enabled", elt.getAttributeValue("enabled"));
     if (elt.getAttributeValue("deprecated") && elt.getAttributeValue("deprecated").search("true") >= 0) {
         button.setPropertyValue("border_style", 9);
         button.setPropertyValue("tooltip", elt.getAttributeValue("description") + " (deprecated)");
    }
}
