#include "src/ui/ProjectManager.h"
#include "src/proj/CEGUIProject.h"
#include "ui_ProjectManager.h"

ProjectManager::ProjectManager(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ProjectManager)
{
    ui->setupUi(this);

    if (ui->view)
    {
        ui->view->sortByColumn(0, Qt::AscendingOrder);
        //self.view.doubleClicked.connect(self.slot_itemDoubleClicked)
    }

    /*
    self.setupContextMenu()

    self.setProject(None)
    */
}

ProjectManager::~ProjectManager()
{
    delete ui;
}

void ProjectManager::setProject(CEGUIProject* project)
{
    setEnabled(!!project);
    ui->view->setModel(project);
}

/*
    def setupContextMenu(self):
        self.view.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)

        self.contextMenu = QtGui.QMenu(self)

        self.createFolderAction = QtGui.QAction(QtGui.QIcon("icons/project_management/create_folder.png"), "Create folder", self)
        self.contextMenu.addAction(self.createFolderAction)
        self.createFolderAction.triggered.connect(self.slot_createFolder)

        self.contextMenu.addSeparator()

        self.addNewFileAction = QtGui.QAction(QtGui.QIcon("icons/project_management/add_new_file.png"), "Add new file", self)
        self.contextMenu.addAction(self.addNewFileAction)
        self.addNewFileAction.triggered.connect(self.slot_addNewFile)

        self.addExistingFileAction = QtGui.QAction(QtGui.QIcon("icons/project_management/add_existing_file.png"), "Add existing file(s)", self)
        self.contextMenu.addAction(self.addExistingFileAction)
        self.addExistingFileAction.triggered.connect(self.slot_addExistingFile)

        self.contextMenu.addSeparator()

        self.renameAction = QtGui.QAction(QtGui.QIcon("icons/project_management/rename.png"), "Rename file/folder", self)
        self.contextMenu.addAction(self.renameAction)
        self.renameAction.triggered.connect(self.slot_renameAction)

        self.removeAction = QtGui.QAction(QtGui.QIcon("icons/project_management/remove.png"), "Remove file(s)/folder(s)", self)
        self.contextMenu.addAction(self.removeAction)
        self.removeAction.triggered.connect(self.slot_removeAction)

        self.view.customContextMenuRequested.connect(self.slot_customContextMenu)

    @staticmethod
    def getItemFromModelIndex(modelIndex):
        # todo: is this ugly thing really the way to do this?

        # Qt says in the docs that returned parent is never None but can be invalid if it's the root
        if not modelIndex.parent().isValid():
            # if it's invalid, we can use the indices as absolute model top level indices
            return modelIndex.model().item(modelIndex.row(), modelIndex.column())
        else:
            # otherwise, resort to recursion
            return ProjectManager.getItemFromModelIndex(modelIndex.parent()).child(modelIndex.row(), modelIndex.column())

    def slot_itemDoubleClicked(self, modelIndex):
        if not modelIndex.model():
            return

        item = ProjectManager.getItemFromModelIndex(modelIndex)
        if item.itemType == Item.File:
            # only react to files, expanding folders is handled by Qt
            self.fileOpenRequested.emit(item.getAbsolutePath())

    def slot_customContextMenu(self, point):
        if self.isEnabled():
            # Qt fails at English a bit?
            selectedIndices = self.view.selectedIndexes()
            # </grammar-nazi-mode>

            # we set everything to disabled and then enable what's relevant
            self.createFolderAction.setEnabled(False)
            self.addNewFileAction.setEnabled(False)
            self.addExistingFileAction.setEnabled(False)
            self.renameAction.setEnabled(False)
            self.removeAction.setEnabled(False)

            if len(selectedIndices) == 0:
                # create root folder
                self.createFolderAction.setEnabled(True)
                self.addNewFileAction.setEnabled(True)
                self.addExistingFileAction.setEnabled(True)

            elif len(selectedIndices) == 1:
                index = selectedIndices[0]
                item = self.getItemFromModelIndex(index)

                if item.itemType == Item.Folder:
                    # create folder inside folder
                    self.createFolderAction.setEnabled(True)
                    self.addNewFileAction.setEnabled(True)
                    self.addExistingFileAction.setEnabled(True)

                self.renameAction.setEnabled(True)
                self.removeAction.setEnabled(True)

            else:
                # more than 1 selected item
                self.removeAction.setEnabled(True)

                #for index in selectedIndices:
                #    item = self.getItemFromModelIndex(index)

        self.contextMenu.exec_(self.mapToGlobal(point))

    def slot_createFolder(self):
        # TODO: Name clashes!

        text, ok = QtGui.QInputDialog.getText(self,
                                              "Create a folder (only affects project file)",
                                              "Name",
                                              QtGui.QLineEdit.Normal,
                                              "New folder")

        if ok:
            item = Item(self.project)
            item.itemType = Item.Folder
            item.name = text

            selectedIndices = self.view.selectedIndexes()

            if len(selectedIndices) == 0:
                self.project.appendRow(item)

            else:
                assert(len(selectedIndices) == 1)

                parent = self.getItemFromModelIndex(selectedIndices[0])
                assert(parent.itemType == Item.Folder)

                parent.appendRow(item)

    def slot_addNewFile(self):
        # TODO: name clashes, duplicates

        file_, _ = QtGui.QFileDialog.getSaveFileName(
            self,
            "Create a new file and add it to the project",
            self.project.getAbsolutePathOf("")
        )

        if file_ == "":
            # user cancelled
            return

        try:
            f = open(file_, "w")
            f.close()

        except OSError:
            QtGui.QMessageBox.question(self,
                                       "Can't create file!",
                                       "Creating file '%s' failed. Exception details follow:\n%s" % (file_, sys.exc_info()[1]),
                                       QtGui.QMessageBox.Ok)

            return

        selectedIndices = self.view.selectedIndexes()

        item = Item(self.project)
        item.itemType = Item.File
        item.path = self.project.getRelativePathOf(file_)

        if len(selectedIndices) == 0:
            self.project.appendRow(item)

        else:
            assert(len(selectedIndices) == 1)

            parent = self.getItemFromModelIndex(selectedIndices[0])
            assert(parent.itemType == Item.Folder)

            parent.appendRow(item)

    def slot_addExistingFile(self):
        # TODO: name clashes, duplicates

        files, _ = QtGui.QFileDialog.getOpenFileNames(self,
                                                           "Select one or more files to add to the project",
                                                           self.project.getAbsolutePathOf(""))
        selectedIndices = self.view.selectedIndexes()

        # lets see if file user wants added isn't already there
        previousResponse = None
        for file_ in files:
            if self.view.model().referencesFilePath(file_):
                # file is already in the project
                response = None

                if previousResponse == QtGui.QMessageBox.YesToAll:
                    response = QtGui.QMessageBox.YesToAll
                elif previousResponse == QtGui.QMessageBox.NoToAll:
                    response = QtGui.QMessageBox.NoToAll
                else:
                    response = QtGui.QMessageBox.question(self, "File is already in the project!",
                            "File '%s' that you are trying to add is already referenced in the "
                            "project.\n\n"
                            "Do you want to add it as a duplicate?" % (file_),
                            QtGui.QMessageBox.Yes | QtGui.QMessageBox.No |
                            QtGui.QMessageBox.YesToAll | QtGui.QMessageBox.NoToAll,
                            QtGui.QMessageBox.Yes)

                    previousResponse = response

                if response in [QtGui.QMessageBox.No, QtGui.QMessageBox.NoToAll]:
                    continue

            item = Item(self.project)
            item.itemType = Item.File
            item.path = self.project.getRelativePathOf(file_)

            if len(selectedIndices) == 0:
                self.project.appendRow(item)

            else:
                assert(len(selectedIndices) == 1)

                parent = self.getItemFromModelIndex(selectedIndices[0])
                assert(parent.itemType == Item.Folder)

                parent.appendRow(item)

    def slot_renameAction(self):
        # TODO: Name clashes!

        selectedIndices = self.view.selectedIndexes()
        assert(len(selectedIndices) == 1)

        item = self.getItemFromModelIndex(selectedIndices[0])
        if item.itemType == Item.File:
            text, ok = QtGui.QInputDialog.getText(self,
                                                  "Rename file (renames the file on the disk!)",
                                                  "New name",
                                                  QtGui.QLineEdit.Normal,
                                                  os.path.basename(item.path))

            if ok and text != os.path.basename(item.path):
                # legit change
                newPath = os.path.join(os.path.dirname(item.path), text)

                try:
                    os.rename(self.project.getAbsolutePathOf(item.path), self.project.getAbsolutePathOf(newPath))
                    item.path = newPath
                    self.project.changed = True

                except OSError:
                    QtGui.QMessageBox.question(self,
                                               "Can't rename!",
                                               "Renaming file '%s' to '%s' failed. Exception details follow:\n%s" % (item.path, newPath, sys.exc_info()[1]),
                                               QtGui.QMessageBox.Ok)

        elif item.itemType == Item.Folder:
            text, ok = QtGui.QInputDialog.getText(self,
                                                  "Rename folder (only affects the project file)",
                                                  "New name",
                                                  QtGui.QLineEdit.Normal,
                                                  item.name)

            if ok and text != item.name:
                item.name = text
                self.project.changed = True

    def slot_removeAction(self):
        if not self.isEnabled():
            return

        selectedIndices = self.view.selectedIndexes()
        # when this is called the selection must not be empty
        assert(len(selectedIndices) > 0)

        removeSpec = ""
        if len(selectedIndices) == 1:
            item = self.getItemFromModelIndex(selectedIndices[0])
            removeSpec = "'%s'" % (item.label)
        else:
            removeSpec = "%i project items" % (len(selectedIndices))

        # we have changes, lets ask the user whether we should dump them or save them
        result = QtGui.QMessageBox.question(self,
                                            "Remove items?",
                                            "Are you sure you want to remove %s from the project? "
                                            "This action can't be undone! "
                                            "(Pressing Cancel will cancel the operation!)" % (removeSpec),
                                            QtGui.QMessageBox.Yes | QtGui.QMessageBox.Cancel,
                                            QtGui.QMessageBox.Cancel)

        if result == QtGui.QMessageBox.Cancel:
            # user chickened out ;-)
            return

        elif result == QtGui.QMessageBox.Yes:
            selectedIndices = sorted(selectedIndices, key = lambda index: index.row(), reverse = True)
            removeCount = 0

            # we have to remove files first because multi-selection could screw us otherwise
            # (Parent also removes it's children)

            # first remove files
            for index in selectedIndices:
                item = self.getItemFromModelIndex(index)

                if not item:
                    continue

                if (item.itemType == Item.File):
                    index.model().removeRow(index.row(), index.parent())
                    removeCount += 1

            # then remove folders
            for index in selectedIndices:
                item = self.getItemFromModelIndex(index)

                if not item:
                    continue

                if (item.itemType == Item.Folder):
                    index.model().removeRow(index.row(), index.parent())
                    removeCount += 1

            if len(selectedIndices) - removeCount > 0:
                logging.error("%i selected project items are unknown and can't be deleted", len(selectedIndices))

            if removeCount > 0:
                self.project.changed = True
*/
