/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2010 The Voreen Team. <http://www.voreen.org>   *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#include "voreen/core/processors/canvasrenderer.h"

#include "voreen/qt/widgets/animation/animationexportwidget.h"
#include "voreen/qt/widgets/animation/timelinewidget.h"
#include "voreen/qt/widgets/animation/animationeditor.h"
#include "voreen/qt/widgets/animation/processortimelinewidget.h"
#include "voreen/qt/widgets/sliderspinboxwidget.h"

#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLCDNumber>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QProgressBar>
#include <QTimeLine>
#include <QSlider>
#include <QTimeLine>
#include <QSlider>
#include <QKeyEvent>
#include <iostream>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QScrollArea>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include <QImageReader>

namespace voreen {

float AnimationEditor::duration_ = 600;

AnimationEditor::AnimationEditor(NetworkEvaluator* eval, Workspace* workspace, QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags | Qt::Tool | Qt::Window)
    , evaluator_(eval)
    , mainMenu_(new QMenuBar(this))
    , currentFrame_(0)
    , frameSkip_(1)
    , timeStretch_(1.0f)
    , interactionMode_(false)
    , workspace_(workspace)
{
    if (workspace->getAnimation()) {
        animation_ = workspace->getAnimation();
        duration_ = (int)animation_->getDuration()*30.0f;
    } else {
        animation_ = new Animation(workspace->getProcessorNetwork());
        animation_->setDuration(20);
        duration_ = 600;
        workspace->setAnimation(animation_);
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    QCheckBox* interaction = new QCheckBox(this);
    connect(interaction, SIGNAL(toggled(bool)), this, SLOT(setInteractionMode(bool)));
    interaction->setText("Interaction Mode");
    QCheckBox* autoPreview = new QCheckBox(this);
    connect(autoPreview, SIGNAL(toggled(bool)), this, SIGNAL(autoPreview(bool)));
    autoPreview->setText("Auto Preview");

    QToolBar* toolbar = new QToolBar(this);
    QToolBar* controlToolbar = new QToolBar(toolbar);
    toolbar->setMovable(true);
    controlToolbar->setMovable(true);

    QToolButton* newButton = new QToolButton(this);
    QToolButton* settingsButton = new QToolButton(this);
    QToolButton* videoExportButton = new QToolButton(this);
    newButton->setIcon(QIcon(":/icons/clear.png"));
    settingsButton->setIcon(QIcon(":/voreenve/icons/show-properties.png"));
    videoExportButton->setIcon(QIcon(":/icons/video_export.png"));

    QToolButton* undoButton = new QToolButton(this);
    QToolButton* redoButton = new QToolButton(this);
    undoButton->setIcon(QIcon(":/icons/edit_undo.png"));
    redoButton->setIcon(QIcon(":/icons/edit_redo.png"));

    QToolButton* startButton = new QToolButton(this);

    QActionGroup* playerControlGroup = new QActionGroup(this);
    QAction* rewind = new QAction(QIcon(":/icons/player_rew.png"), tr("rewind"), this);
    QAction* play = new QAction(QIcon(":/icons/player_play.png"), tr("play"), this);
    QAction* pause = new QAction(QIcon(":/icons/player_pause.png"), tr("pause"), this);
    QAction* stop = new QAction(QIcon(":/icons/player_stop.png"), tr("stop"), this);
    QAction* forward = new QAction(QIcon(":/icons/player_fwd.png"), tr("forward"), this);
    playerControlGroup->addAction(rewind);
    playerControlGroup->addAction(play);
    playerControlGroup->addAction(pause);
    playerControlGroup->addAction(stop);
    playerControlGroup->addAction(forward);
    rewind->setCheckable(true);
    play->setCheckable(true);
    pause->setCheckable(true);
    stop->setCheckable(true);
    forward->setCheckable(true);
    connect(playerControlGroup, SIGNAL(triggered(QAction*)), this, SLOT(playerControl(QAction*)));
    QToolButton* endButton = new QToolButton(this);

    QToolButton* recordButton = new QToolButton(this);

    startButton->setIcon(QIcon(":/icons/player_start.png"));
    endButton->setIcon(QIcon(":/icons/player_end.png"));

    recordButton->setIcon(QIcon(":/icons/player_record.png"));

    toolbar->addWidget(newButton);
    toolbar->addWidget(settingsButton);
    toolbar->addWidget(videoExportButton);
    newButton->setToolTip(tr("New Animation"));
    settingsButton->setToolTip(tr("Animation Settings"));
    videoExportButton->setToolTip(tr("Export Animation"));

    toolbar->addSeparator();

    toolbar->addWidget(undoButton);
    toolbar->addWidget(redoButton);
    undoButton->setToolTip(tr("Undo"));
    redoButton->setToolTip(tr("Redo"));

    toolbar->addSeparator();

    controlToolbar->addWidget(startButton);
    controlToolbar->addAction(rewind);
    controlToolbar->addAction(pause);
    controlToolbar->addAction(play);
    controlToolbar->addAction(stop);
    controlToolbar->addAction(forward);
    controlToolbar->addWidget(endButton);
    controlToolbar->addWidget(recordButton);

    startButton->setToolTip(tr("Start"));
    rewind->setToolTip(tr("Rewind"));
    pause->setToolTip(tr("Pause"));
    play->setToolTip(tr("Play"));
    stop->setToolTip(tr("Stop"));
    forward->setToolTip(tr("Forward"));
    endButton->setToolTip(tr("End"));
    recordButton->setToolTip(tr("Take Snapshot"));

    toolbar->addWidget(controlToolbar);

    mainLayout->addWidget(toolbar);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addSpacing(8);
    topLayout->addWidget(interaction);
    topLayout->addSpacing(10);
    topLayout->addWidget(autoPreview);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    TimelineWidget* timelineWidget = new TimelineWidget(animation_, this, evaluator_);
    mainLayout->addWidget(timelineWidget);

    connect(this, SIGNAL(currentFrameChanged(int)), timelineWidget, SLOT(currentFrame(int)));
    connect(this, SIGNAL(currentFrameChanged(int)), timelineWidget, SIGNAL(currentFrameChanged(int)));
    connect(this, SIGNAL(durationChanged(int)), timelineWidget, SIGNAL(durationChanged(int)));
    connect(this, SIGNAL(autoPreview(bool)), timelineWidget, SIGNAL(autoPreview(bool)));

    connect(this, SIGNAL(newAnimation(Animation*)), timelineWidget, SLOT(rebuildAnimation(Animation*)));
    connect(timelineWidget, SIGNAL(recordAt(int)), this, SLOT(recordAt(int)));
    connect(timelineWidget, SIGNAL(setAnimationEditorDuration(int)), this, SLOT(setDuration(int)));

    connect(newButton, SIGNAL(clicked()), this, SLOT(newAnimation()));
    connect(settingsButton, SIGNAL(clicked()), this, SLOT(settings()));
    connect(videoExportButton, SIGNAL(clicked()), this, SLOT(videoExport()));

    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(endButton, SIGNAL(clicked()), this, SLOT(end()));
    connect(recordButton, SIGNAL(clicked()), this, SLOT(record()));
    connect(this, SIGNAL(recordSignal()), timelineWidget, SIGNAL(recordSignal()));
    connect(this, SIGNAL(recordSignal()), timelineWidget, SLOT(checkForChanges()));
    connect(undoButton, SIGNAL(clicked()), this, SLOT(undo()));
    connect(redoButton, SIGNAL(clicked()), this, SLOT(redo()));

    init();
}

float AnimationEditor::getDuration() {
    return duration_;
}

void AnimationEditor::setWorkspace(Workspace* workspace) {
    workspace_ = workspace;
    if(workspace->getAnimation()) {
        animation_ = workspace->getAnimation();
        emit newAnimation(animation_);
    }
    else if(evaluator_ && evaluator_->getProcessorNetwork()){
        animation_ = new Animation(const_cast<ProcessorNetwork*>(evaluator_->getProcessorNetwork()));
        workspace->setAnimation(animation_);
        animation_->setDuration(20);
        emit newAnimation(animation_);
    }
}

void AnimationEditor::init() {
    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
    emit durationChanged(static_cast<int>(duration_));

}

void AnimationEditor::playerControl(QAction*action) {
    if(action->text() == "rewind") {
        rewind();
    }
    else if(action->text() == "play") {
        play();
    }
    else if(action->text() == "pause") {
        pause();
    }
    else if(action->text() == "stop") {
        stop();
    }
    else if(action->text() == "forward") {
        forward();
    }
}

void AnimationEditor::newAnimation() {
    animation_ = new Animation(const_cast<ProcessorNetwork*>(evaluator_->getProcessorNetwork()));
    workspace_->setAnimation(animation_);
    animation_->setDuration(20);
    emit newAnimation(animation_);
}

void AnimationEditor::videoExport() {
    AnimationExportWidget* animationExportWidget
        = new AnimationExportWidget(this, animation_, evaluator_, duration_, 1, int(duration_), timeStretch_);
    animationExportWidget->resize(200,150);
    animationExportWidget->networkChanged();
    animationExportWidget->exec();
}

void AnimationEditor::undo() {
    animation_->undoLastChange();
}

void AnimationEditor::redo() {
    animation_->redoLastUndo();
}

void AnimationEditor::start() {
    currentFrame_  = 0;
    emit currentFrameChanged(currentFrame_);
}

void AnimationEditor::rewind() {
    if(interactionMode_)
        animation_->setInteractionMode(true);
    frameSkip_ = -4;
    timer_->start(30);
}

void AnimationEditor::pause() {
    if(interactionMode_)
        animation_->setInteractionMode(false);
    animation_->renderAt((float)currentFrame_/30.0f);
    timer_->stop();
}

void AnimationEditor::play() {
    if(interactionMode_)
        animation_->setInteractionMode(true);
    frameSkip_ = 1;
    timer_->start(30);      // renders every 3/100 second
}

void AnimationEditor::stop() {
    if(interactionMode_)
        animation_->setInteractionMode(false);
    timer_->stop();
    currentFrame_ = 0;
    animation_->renderAt((float)currentFrame_/30.0f);
    emit currentFrameChanged(currentFrame_);
}

void AnimationEditor::forward() {
    if(interactionMode_)
        animation_->setInteractionMode(true);
    frameSkip_ = 4;
    timer_->start(30);
}
void AnimationEditor::end() {
    if(interactionMode_)
        animation_->setInteractionMode(false);
    currentFrame_ = static_cast<int>(duration_);
    timer_->stop();
    animation_->renderAt((float)currentFrame_/30.0f);
    emit currentFrameChanged(currentFrame_);
}

void AnimationEditor::update() {
    if(interactionMode_)
        animation_->setInteractionMode(true);
    else
        animation_->setInteractionMode(false);
    if(currentFrame_ + frameSkip_ < 0) {
        currentFrame_ = 0;
        timer_->stop();
    }
    else if(currentFrame_ + frameSkip_ > duration_) {
        currentFrame_ = static_cast<int>(duration_);
        timer_->stop();
    }
    else
        currentFrame_ +=frameSkip_*timeStretch_;
    emit currentFrameChanged(currentFrame_);
    animation_->renderAt((float)currentFrame_/30.0f);
}

void AnimationEditor::setCurrentFrame(int currentFrame) {
    currentFrame_ = currentFrame;
    animation_->renderAt((float)currentFrame_/30.0f);
    emit currentFrameChanged(currentFrame_);
    timer_->stop();
}

void AnimationEditor::setInteractionMode(bool on) {
    interactionMode_ = on;
}

void AnimationEditor::record() {
    animation_->setActualNetworkAsKeyvalues((float)currentFrame_/30.0f);
    emit recordSignal();
}

void AnimationEditor::recordAt(int frame) {
    animation_->setActualNetworkAsKeyvalues((float)frame/30.f);
    emit recordSignal();
}

void AnimationEditor::setDuration(int duration) {
    duration_ = duration;
    emit durationChanged(duration);
    animation_->setDuration((float)duration /30.0f);
}

void AnimationEditor::settings() {
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Animation Settings"));
    QVBoxLayout* lay = new QVBoxLayout(dialog);
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QGroupBox* durationBox = new QGroupBox(dialog);
    durationBox->setTitle("Duration (seconds)");
    QHBoxLayout* durationLayout = new QHBoxLayout(durationBox);
    QSpinBox dsp(dialog);
    dsp.setMinimum(1);
    dsp.setMaximum(200000);
    dsp.setValue(static_cast<int>(duration_/ 30.0f));
    durationLayout->addWidget(&dsp);
    QPushButton ok("Ok", dialog);
    QPushButton cancel("Cancel", dialog);
    connect (&ok, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect (&cancel, SIGNAL(clicked()), dialog, SLOT(reject()));

    QLabel* tsLabel = new QLabel("Time Stretch Factor:");
    QHBoxLayout* timeStretchLayout = new QHBoxLayout();
    DoubleSliderSpinBoxWidget* timeStretchSlider = new DoubleSliderSpinBoxWidget(this);
    timeStretchSlider->setMaxValue(5);
    timeStretchSlider->setSingleStep(0.1f);
    timeStretchSlider->setValue(1.0f);
    timeStretchSlider->setMaximumWidth(100);
    connect(timeStretchSlider, SIGNAL(valueChanged(double)), this, SLOT(timeStretchChanged(double)));
    timeStretchLayout->addWidget(tsLabel);
    timeStretchLayout->addWidget(timeStretchSlider);
    timeStretchLayout->addStretch();
    lay->addLayout(timeStretchLayout);

    buttonLayout->addWidget(&ok);
    buttonLayout->addWidget(&cancel);

    lay->addWidget(durationBox);
    lay->addLayout(buttonLayout);
    if (dialog->exec() == QDialog::Accepted)
        setDuration(static_cast<int>(dsp.value()* 30.0f));
}

void AnimationEditor::timeStretchChanged(double stretch) {
    timeStretch_ = stretch;
}

} // namespace voreen
