#include "src/ui/imageset/ImagesetEntry.h"
#include "src/ui/imageset/ImageEntry.h"
#include "src/util/Utils.h"
#include "qfilesystemwatcher.h"
#include "qcursor.h"
#include "qdom.h"
#include "qpen.h"

ImagesetEntry::ImagesetEntry(QGraphicsItem* parent)
    : QGraphicsPixmapItem(/*parent*/)
{
    setShapeMode(BoundingRectShape);
    setCursor(Qt::ArrowCursor);
/*
        self.visual = visual

        self.displayingReloadAlert = False
*/

    transparencyBackground = new QGraphicsRectItem(this);
    transparencyBackground->setFlags(ItemStacksBehindParent);
    transparencyBackground->setBrush(Utils::getCheckerboardBrush());
    transparencyBackground->setPen(QPen(QColor(Qt::transparent)));
}

ImagesetEntry::~ImagesetEntry()
{
    delete imageMonitor;
}

void ImagesetEntry::loadFromElement(const QDomElement& xml)
{
    _name = xml.attribute("name", "Unknown");

    loadImage(xml.attribute("imagefile", ""));

    nativeHorzRes = xml.attribute("nativeHorzRes", "800").toInt();
    nativeVertRes = xml.attribute("nativeVertRes", "600").toInt();

    autoScaled = xml.attribute("autoScaled", "false");

    auto xmlImage = xml.firstChildElement("Image");
    while (!xmlImage.isNull())
    {
        ImageEntry* image = new ImageEntry(this);
        image->loadFromElement(xmlImage);
        imageEntries.push_back(image);

        xmlImage = xmlImage.nextSiblingElement("Image");
    }
}

void ImagesetEntry::saveToElement(QDomElement& xml)
{
    xml.setTagName("Imageset");
    xml.setAttribute("version", "2");

    xml.setAttribute("name", _name);
    xml.setAttribute("imagefile", imageFile);

    xml.setAttribute("nativeHorzRes", QString::number(nativeHorzRes));
    xml.setAttribute("nativeVertRes", QString::number(nativeVertRes));
    xml.setAttribute("autoScaled", autoScaled);

    for (auto& image : imageEntries)
    {
        auto xmlImage = xml.ownerDocument().createElement("Image");
        image->saveToElement(xmlImage);
        xml.appendChild(xmlImage);
    }
}

ImageEntry* ImagesetEntry::getImageEntry(const QString& name) const
{
    auto it = std::find_if(imageEntries.begin(), imageEntries.end(), [&name](ImageEntry* ent)
    {
        return ent->name() == name;
    });

    assert(it != imageEntries.end());
    return (it != imageEntries.end()) ? (*it) : nullptr;
}

//Returns an absolute (OS specific!) path of the underlying image
QString ImagesetEntry::getAbsoluteImageFile() const
{
/*
        return os.path.join(os.path.dirname(self.visual.tabbedEditor.filePath), self.imageFile)
*/
    return "";
}

// Converts given absolute underlying image path relative to the directory where the .imageset file resides
QString ImagesetEntry::convertToRelativeImageFile(const QString& absPath) const
{
/*
        return os.path.normpath(os.path.relpath(absoluteImageFile, os.path.dirname(self.visual.tabbedEditor.filePath)))
*/
    return "";
}

// Monitor the image with a QFilesystemWatcher, ask user to reload if changes to the file were made
void ImagesetEntry::onImageChangedByExternalProgram()
{
    //???really here?
/*
if not self.displayingReloadAlert:
    self.displayingReloadAlert = True
    ret = QtGui.QMessageBox.question(self.visual.tabbedEditor.mainWindow,
                                     "Underlying image '%s' has been modified externally!" % (self.imageFile),
                                     "The file has been modified outside the CEGUI Unified Editor.\n\nReload the file?\n\nIf you select Yes, UNDO HISTORY MIGHT BE PARTIALLY BROKEN UNLESS THE NEW SIZE IS THE SAME OR LARGER THAN THE OLD!",
                                     QtGui.QMessageBox.No | QtGui.QMessageBox.Yes,
                                     QtGui.QMessageBox.No) # defaulting to No is safer IMO

    if ret == QtGui.QMessageBox.Yes:
        self.loadImage(self.imageFile)

    elif ret == QtGui.QMessageBox.No:
        pass

    else:
        # how did we get here?
        assert(False)

    self.displayingReloadAlert = False
*/
}

// Replaces the underlying image (if any is loaded) to the image on given relative path
// Relative path is relative to the directory where the .imageset file resides
// (which is usually your project's imageset resource group path)
void ImagesetEntry::loadImage(const QString& relPath)
{
    // If imageMonitor is null, then no images are being watched or the
    // editor is first being opened up
    // Otherwise, the image is being changed or switched, and the monitor
    // should update itself accordingly
    if (imageMonitor) imageMonitor->removePath(getAbsoluteImageFile());

    QString absPath = getAbsoluteImageFile();

    imageFile = relPath;
    setPixmap(QPixmap(absPath));
    transparencyBackground->setRect(boundingRect());

    // Go over all image entries and set their position to force them to be constrained
    // to the new pixmap's dimensions
    for (auto& imageEntry : imageEntries)
    {
/*
        imageEntry.setPos(imageEntry.pos())
        imageEntry.updateDockWidget()
*/
    }

/*
    self.visual.refreshSceneRect()
*/
    if (!imageMonitor)
    {
        imageMonitor = new QFileSystemWatcher();
        //connect(imageMonitor, &QFileSystemWatcher::fileChanged, this, &ImagesetEntry::onImageChangedByExternalProgram);
    }
    imageMonitor->addPath(absPath);
}
