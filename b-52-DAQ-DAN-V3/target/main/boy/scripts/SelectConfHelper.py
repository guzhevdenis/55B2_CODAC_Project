from javax.swing import JFileChooser
from javax.swing.filechooser import FileFilter
from java.io import File
from java.lang import System
from java.lang import String

class XmlFileFilter(FileFilter):
    def getDescription(self):
        return "XML files"
    
    def accept(self, f):
        if f.isDirectory():
            return True
        
        ext = None
        # Python string is returned, not Java String!
        s = f.getName()
        i = s.rfind('.')
        if i >= 0:
            ext = s[i+1 : ].lower()
        
        if ext == "xml":
            return True
        return False

def doAction(display, WIDGET_TEXT_FILE1, WIDGET_TEXT_FILE2, ENV_SUP_CONFIG_PATH, DEFAULT_SUP_CONFIG_PATH):
    fc = JFileChooser()
    fc.setDialogTitle("Select config file")
    fc.setApproveButtonText("Select")
    fc.addChoosableFileFilter(XmlFileFilter())
    fc.setAcceptAllFileFilterUsed(False)
    try:
        supConfigPath = System.getenv(ENV_SUP_CONFIG_PATH)
    except NameError:
        supConfigPath = DEFAULT_SUP_CONFIG_PATH
    if supConfigPath is None:
        supConfigPath = DEFAULT_SUP_CONFIG_PATH
    fc.setCurrentDirectory(File(supConfigPath))
    
    returnVal = fc.showOpenDialog(None)
    if returnVal == JFileChooser.APPROVE_OPTION:
        file = fc.getSelectedFile()
        if file is not None:
            fileName = file.getName()
            confName = String.substring(fileName, 0, String.length(fileName)-4)
            display.getWidget(WIDGET_TEXT_FILE1).setPropertyValue("text", confName)
            if WIDGET_TEXT_FILE2 != WIDGET_TEXT_FILE1:
                display.getWidget(WIDGET_TEXT_FILE2).setPropertyValue("text", confName)

