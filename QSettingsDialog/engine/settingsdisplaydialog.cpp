#include "settingsdisplaydialog.h"
#include "ui_settingsdisplaydialog.h"
#include <dialogmaster.h>

SettingsDisplayDialog::SettingsDisplayDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDisplayDialog),
	delegate(nullptr),
	maxWidthBase(0)
{
	ui->setupUi(this);
	DialogMaster::masterDialog(this);

	connect(this->ui->buttonBox, &QDialogButtonBox::clicked,
			this, &SettingsDisplayDialog::buttonBoxClicked);

	int listSpacing = this->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
	this->delegate = new CategoryItemDelegate(std::bind(&SettingsDisplayDialog::updateWidth, this, std::placeholders::_1),
											  this->ui->categoryListWidget->iconSize(),
											  qMax(qRound(listSpacing * (2./3.)), 1),
											  this);
	this->ui->categoryListWidget->setItemDelegate(this->delegate);
	this->ui->categoryListWidget->setSpacing(qMax(qRound(listSpacing / 3.), 1) - 1);

	int spacing = this->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
	this->ui->contentLayout->setSpacing(spacing);
	this->ui->categoryLineSpacer->changeSize(spacing,
											 0,
											 QSizePolicy::Fixed,
											 QSizePolicy::Fixed);

	this->resetListSize();
}

SettingsDisplayDialog::~SettingsDisplayDialog()
{
	delete ui;
}

void SettingsDisplayDialog::resetListSize()
{
	int max = this->ui->categoryListWidget->count();
	if(max == 0) {
		this->ui->categoryContentWidget->hide();
		this->resize(this->width() - this->ui->categoryContentWidget->sizeHint().width(),
					 this->height());
	} else {
		auto width = this->ui->categoryListWidget->sizeHint().width();
		this->ui->categoryListWidget->setFixedWidth(width);
		this->maxWidthBase = width;
	}
}

void SettingsDisplayDialog::updateWidth(int width)
{
	if(width > this->maxWidthBase) {
		this->maxWidthBase = width;
		QStyle *style = this->ui->categoryListWidget->style();
		width += style->pixelMetric(QStyle::PM_ScrollBarExtent);
		width += style->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
		this->ui->categoryListWidget->setFixedWidth(width);
	}
}

void SettingsDisplayDialog::buttonBoxClicked(QAbstractButton *button)
{
	switch(this->ui->buttonBox->standardButton(button)) {
	case QDialogButtonBox::Ok:
		emit save(true);
		//DEBUG
		this->accept();
		break;
	case QDialogButtonBox::Cancel:
		this->reject();
		break;
	case QDialogButtonBox::Apply:
		emit save(false);
		break;
	case QDialogButtonBox::RestoreDefaults:
		if(DialogMaster::warning(this,
								 tr("All custom changes will be deleted and the defaults restored. "
									"<i>This cannot be undone!</i>"),
								 tr("%1?").arg(button->text()),
								 QString(),
								 QMessageBox::Yes | QMessageBox::No,
								 QMessageBox::Yes,
								 QMessageBox::No)
		   == QMessageBox::Yes) {
			emit reset();
			//DEBUG
			this->accept();
		}
		break;
	default:
		Q_UNREACHABLE();
	}
}



CategoryItemDelegate::CategoryItemDelegate(std::function<void (int)> updateFunc, const QSize &iconSize, int layoutSpacing, QObject *parent) :
	QStyledItemDelegate(parent),
	iconSize(),
	updateFunc(updateFunc)
{
	this->iconSize = iconSize + QSize(0, layoutSpacing);
}

QSize CategoryItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size = this->QStyledItemDelegate::sizeHint(option, index);
	this->updateFunc(size.width());
	return size.expandedTo(this->iconSize);
}
