/********************************************************************************
** Form generated from reading UI file 'aboutbox.ui'
**
** Created: Fri Nov 5 19:24:32 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTBOX_H
#define UI_ABOUTBOX_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_VoreenAboutBox
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QLabel *label_6;
    QHBoxLayout *hboxLayout;
    QLabel *labelLeft;
    QSpacerItem *spacerItem;
    QLabel *labelRight;
    QLabel *labelMainDevelopers;
    QLabel *labelDevelopers;
    QLabel *label_7;
    QLabel *label_5;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem1;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *VoreenAboutBox)
    {
        if (VoreenAboutBox->objectName().isEmpty())
            VoreenAboutBox->setObjectName(QString::fromUtf8("VoreenAboutBox"));
        VoreenAboutBox->resize(525, 438);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(VoreenAboutBox->sizePolicy().hasHeightForWidth());
        VoreenAboutBox->setSizePolicy(sizePolicy);
        VoreenAboutBox->setMinimumSize(QSize(437, 307));
        VoreenAboutBox->setMaximumSize(QSize(800, 500));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(212, 208, 200, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(234, 229, 220, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(128, 128, 128, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(141, 139, 133, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        QBrush brush6(QColor(181, 195, 212, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush6);
        QBrush brush7(QColor(64, 64, 64, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush7);
        QBrush brush8(QColor(10, 36, 106, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Highlight, brush8);
        palette.setBrush(QPalette::Active, QPalette::HighlightedText, brush2);
        QBrush brush9(QColor(0, 0, 255, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Link, brush9);
        QBrush brush10(QColor(255, 0, 255, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::LinkVisited, brush10);
        QBrush brush11(QColor(232, 232, 232, 255));
        brush11.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush11);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush7);
        palette.setBrush(QPalette::Inactive, QPalette::Highlight, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Link, brush9);
        palette.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush10);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush11);
        QBrush brush12(QColor(106, 104, 100, 255));
        brush12.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush12);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        QBrush brush13(QColor(233, 231, 227, 255));
        brush13.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush13);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush12);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush12);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Highlight, brush8);
        palette.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Link, brush9);
        palette.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush10);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush11);
        VoreenAboutBox->setPalette(palette);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/voreenve/icons/voreen-logo_64x64.png"), QSize(), QIcon::Normal, QIcon::Off);
        VoreenAboutBox->setWindowIcon(icon);
        VoreenAboutBox->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(VoreenAboutBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame = new QFrame(VoreenAboutBox);
        frame->setObjectName(QString::fromUtf8("frame"));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush2);
        QBrush brush14(QColor(1, 101, 151, 255));
        brush14.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush14);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush14);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush14);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush14);
        frame->setPalette(palette1);
        frame->setAutoFillBackground(true);
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Plain);
        frame->setLineWidth(0);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_6 = new QLabel(frame);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMinimumSize(QSize(510, 86));
        label_6->setLineWidth(0);
        label_6->setPixmap(QPixmap(QString::fromUtf8(":/voreenve/image/aboutbox.png")));
        label_6->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_6);


        verticalLayout->addWidget(frame);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        labelLeft = new QLabel(VoreenAboutBox);
        labelLeft->setObjectName(QString::fromUtf8("labelLeft"));
        labelLeft->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        hboxLayout->addWidget(labelLeft);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        labelRight = new QLabel(VoreenAboutBox);
        labelRight->setObjectName(QString::fromUtf8("labelRight"));
        labelRight->setTextFormat(Qt::RichText);
        labelRight->setScaledContents(false);
        labelRight->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);
        labelRight->setOpenExternalLinks(true);

        hboxLayout->addWidget(labelRight);


        verticalLayout->addLayout(hboxLayout);

        labelMainDevelopers = new QLabel(VoreenAboutBox);
        labelMainDevelopers->setObjectName(QString::fromUtf8("labelMainDevelopers"));
        labelMainDevelopers->setScaledContents(false);
        labelMainDevelopers->setWordWrap(true);

        verticalLayout->addWidget(labelMainDevelopers);

        labelDevelopers = new QLabel(VoreenAboutBox);
        labelDevelopers->setObjectName(QString::fromUtf8("labelDevelopers"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelDevelopers->sizePolicy().hasHeightForWidth());
        labelDevelopers->setSizePolicy(sizePolicy1);
        labelDevelopers->setScaledContents(true);
        labelDevelopers->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        labelDevelopers->setWordWrap(true);

        verticalLayout->addWidget(labelDevelopers);

        label_7 = new QLabel(VoreenAboutBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMinimumSize(QSize(507, 52));
        label_7->setAutoFillBackground(false);
        label_7->setWordWrap(true);
        label_7->setOpenExternalLinks(false);

        verticalLayout->addWidget(label_7);

        label_5 = new QLabel(VoreenAboutBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAutoFillBackground(false);
        label_5->setOpenExternalLinks(false);

        verticalLayout->addWidget(label_5);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        spacerItem1 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);

        okButton = new QPushButton(VoreenAboutBox);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setDefault(true);
        okButton->setFlat(false);

        hboxLayout1->addWidget(okButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(horizontalSpacer);


        verticalLayout->addLayout(hboxLayout1);


        retranslateUi(VoreenAboutBox);
        QObject::connect(okButton, SIGNAL(clicked()), VoreenAboutBox, SLOT(accept()));

        QMetaObject::connectSlotsByName(VoreenAboutBox);
    } // setupUi

    void retranslateUi(QDialog *VoreenAboutBox)
    {
        VoreenAboutBox->setWindowTitle(QApplication::translate("VoreenAboutBox", "About", 0, QApplication::UnicodeUTF8));
        label_6->setText(QString());
        labelLeft->setText(QApplication::translate("VoreenAboutBox", "Volume Rendering Engine __VERSION__\n"
"__APPLICATION__\n"
"__COPYRIGHT__", 0, QApplication::UnicodeUTF8));
        labelRight->setText(QApplication::translate("VoreenAboutBox", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">__SVNVERSION__</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; text-decoration: underline; color:#0000ff;\"><a href=\"http://www.voreen.org\">http://www.voreen.org</a></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelMainDevelopers->setText(QApplication::translate("VoreenAboutBox", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Concept &amp; Design: </span>__MAINDEVELOPERS__</p></body></html>", 0, QApplication::UnicodeUTF8));
        labelDevelopers->setText(QApplication::translate("VoreenAboutBox", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Developers: </span>__DEVELOPERS__</p></body></html>", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("VoreenAboutBox", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">License: </span>You may use, distribute and copy the Voreen software package under the terms of the GNU General Public License version 2, see the files LICENSE.txt and LICENSE-academic.txt for details.</p></body></html>", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("VoreenAboutBox", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:9pt; font-weight:600;\">Contact:</span><span style=\" font-size:9pt;\"> Timo Ropinski (</span><a href=\"mailto:ropinski@math.uni-muenster.de\"><span style=\" font-size:9pt; text-decoration: underline; color:#0000ff;\">ropinski@math.uni-muenster.de</span></a><span style=\" font-size:9pt;\">)</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("VoreenAboutBox", "OK", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class VoreenAboutBox: public Ui_VoreenAboutBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTBOX_H
