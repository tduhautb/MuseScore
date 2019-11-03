/*
 * ==================================================================
 *
 *    Filename:  LilyExportDialog.hpp
 *
 *    Description:  Header for the options dialog window class of the LilyPond exporter
 *
 *    Currently supports :
 *    * output language option : english (a - g) or italiano (do - si)
 *    * number of files to output : single-file (default, implemented) or multi-files (not
 * implemented)
 *
 *    Author:  Thibaud Duhautbout (TD), thibaud@duhautbout.ovh
 *
 * ==================================================================
 */

#include "exportly.hpp"

#include <QDialog>

namespace Ms
{
class LilyExportDialog : public QDialog
{
    Q_OBJECT

  private:
    LilyExporter::LilyExportOptions _options;

    QPushButton* _validateButton;
    QVBoxLayout* _vBoxLayout;
    QRadioButton* _langRadioButtonEN;
    QRadioButton* _langRadioButtonIT;
    QButtonGroup* _langButtonGroup;
    QRadioButton* _oneFileRadioButton;
    QRadioButton* _multiFilesRadioButton;
    QButtonGroup* _nbFilesButtonGroup;

  private slots:
    void validateOptions();

  public:
    LilyExportDialog(QMainWindow* mscore);
    const LilyExporter::LilyExportOptions& getOptions() const;
    virtual ~LilyExportDialog();
};
} // namespace Ms
