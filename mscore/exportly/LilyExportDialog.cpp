/*
 * ==================================================================
 *
 *    Filename:  LilyExportDialog.cpp
 *
 *    Description:
 *    TODO options to add :
 *    * checkbox to remove engraving at the end
 *    * size of the staves
 *    * compress full bar rests and format
 *
 *    TODO global management :
 *    * manage translations of the labels in the window
 *
 *    Author:  Thibaud Duhautbout (TD), thibaud@duhautbout.ovh
 *
 * ==================================================================
 */

#include <QButtonGroup>
#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>

#include "LilyExportDialog.hpp"

namespace Ms
{

LilyExportDialog::LilyExportDialog(QMainWindow* mscore) : QDialog(mscore)
{
    setModal(true);

    _vBoxLayout = new QVBoxLayout();

    _langButtonGroup = new QButtonGroup(this);

    QLabel* _label = new QLabel("Lang of the export :");
    _vBoxLayout->addWidget(_label);

    // TODO : manage translations
    _langRadioButtonIT = new QRadioButton("Italiano (do - si)");
    _langRadioButtonIT->setChecked(true);
    _langRadioButtonEN = new QRadioButton("English (A - G)");
    _langButtonGroup->addButton(_langRadioButtonIT, Ms::LilyExporter::ITALIANO);
    _langButtonGroup->addButton(_langRadioButtonEN, Ms::LilyExporter::ENGLISH);
    _vBoxLayout->addWidget(_langRadioButtonEN);
    _vBoxLayout->addWidget(_langRadioButtonIT);

    _oneFileRadioButton = new QRadioButton("Export the music in one single file");
    _oneFileRadioButton->setChecked(true);
    _multiFilesRadioButton =
        new QRadioButton("Export the music in separate files for each instrument");
    _nbFilesButtonGroup = new QButtonGroup(this);
    _nbFilesButtonGroup->addButton(_oneFileRadioButton, 1);
    _nbFilesButtonGroup->addButton(_multiFilesRadioButton, 0);

    QLabel* _label2 = new QLabel("File repartition of the export :");
    _vBoxLayout->addWidget(_label2);

    _vBoxLayout->addWidget(_oneFileRadioButton);
    _vBoxLayout->addWidget(_multiFilesRadioButton);

    _validateButton = new QPushButton("Validate");
    _vBoxLayout->addWidget(_validateButton);
    QObject::connect(_validateButton, SIGNAL(clicked()), this, SLOT(validateOptions()));

    this->setLayout(_vBoxLayout);
}

void LilyExportDialog::validateOptions()
{
    _options._lang = Ms::LilyExporter::OutputLanguage(_langButtonGroup->checkedId());
    _options._singleFile = _nbFilesButtonGroup->checkedId();
    accept();
}

LilyExportDialog::~LilyExportDialog()
{
}
} // namespace Ms
