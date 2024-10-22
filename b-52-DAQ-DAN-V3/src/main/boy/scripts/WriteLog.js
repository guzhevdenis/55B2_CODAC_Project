importPackage(Packages.org.csstudio.opibuilder.scriptUtil);

var pv = PVUtil.getString(triggerPV);
var pvname = widget.getPropertyValue("pv_name")
display.getWidget("LogWidget").setPropertyValue("text",display.getWidget("LogWidget").getPropertyValue("text") + "\n" + pvname + " = " + pv );
