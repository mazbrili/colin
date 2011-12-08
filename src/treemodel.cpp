#include "treemodel.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QPalette>
#include "colinstruct.h"
#include "unitsettings.h"
#include "colinicons.h"

QBrush HeaderBrush;
QFont HeaderFont;


treeModel::treeModel(QObject *parent) :
    QAbstractItemModel(parent)
{


	HeaderFont = QApplication::font();
	HeaderFont.setBold(true);

	QLinearGradient lg(0, 0, 0, 24);
	lg.setColorAt(0.,			QApplication::palette("QHeaderView").color(QPalette::Light));
	lg.setColorAt(1.,			QApplication::palette("QHeaderView").color(QPalette::Background));
	QPixmap pm(1000, 24);
	QPainter p(&pm);
	p.setBrush(QBrush(lg));
	QPen pen;
	lg.setColorAt(0.,			QApplication::palette("QWidget").color(QPalette::Dark));
	lg.setColorAt(0.5,			QApplication::palette("QWidget").color(QPalette::Dark).darker(180));
	lg.setColorAt(1,			QApplication::palette("QWidget").color(QPalette::Dark));
	pen.setBrush(QBrush(lg));
	p.setPen(pen);
	p.drawRect(0, 0, 1000, 23);
	HeaderBrush = QBrush(pm);

	newNodeBuffer[0]=tr("x");
	newNodeBuffer[1]=tr("z");

	newBeamBuffer[0]=tr("from");
	newBeamBuffer[1]=tr("to");
	newBeamBuffer[2]=tr("material");
	newBeamBuffer[3]=tr("cross section");

	newLoadFormBuffer = ColinLoad::nodeLoad;
	setLoadBuffer(newLoadBuffer, newLoadFormBuffer);
	t = 0;
	connect(&filelist::instance(),				SIGNAL(currentChanged(ColinStruct*)),
			this,								SLOT(setStruct(ColinStruct*)));
}


QModelIndex treeModel::index(int row, int column, const QModelIndex & parent) const
{
#ifdef TREEVERBOSE
	qDebug() << "treeModel::index(" << parent << " / " << row << " / " << column <<  ")";
#endif
	if(!parent.isValid())
		return createIndex(row, column, lists);

	return createIndex(row, column, childId(modelIds(parent.internalId()), parent.row()));
}

QModelIndex treeModel::parent(const QModelIndex & index) const
{
#ifdef TREEVERBOSE
	qDebug() << "treeModel::parent(" << index << ")";
#endif

	if(index.internalId() == lists)
		return QModelIndex();

	return createIndex(listindex(index), 0, parentId(modelIds(index.internalId())));
}

int treeModel::rowCount(const QModelIndex & parent) const
{
#ifdef TREEVERBOSE
	qDebug() << "treeModel::rowCount(" << parent << ")";
#endif

	if(!parent.isValid())
		return 5;
	switch(parent.internalId())
	{
	case lists:
		switch(parent.row())
		{
		case 0:					return t->node_n()+1;
		case 1:					return t->beam_n()+1;
		case 2:					return t->load_n()+1;
		case 3:					return t->bls_n()+1;
		case 4:					return t->cls_n()+1;
		default:				return 0;
		}
	case cls:					return t->cls(parent.row()).count()+1;
	case beam:					return 6;
		default:					return 1;
	}
}

int treeModel::columnCount(const QModelIndex & parent) const
{
#ifdef TREEVERBOSE
	qDebug() << "treeModel::columnCount(" << parent << ")";
#endif

	switch(parent.internalId())
	{
	case lists:
		switch(parent.row())
		{
		case 0:						return 3;
		case 1:						return 5;
		case 2:						return 6;
		case 3:						return 3;
		case 4:						return 2;
		default:					return 6;
		}
	case cls:						return 3;
	case node:						return 4;
	case beam:						return 1;
	default:						return 6;
	}
}

QVariant treeModel::data(const QModelIndex & index, int role) const
{
#ifdef TREEVERBOSE
	qDebug() << "treeModel::data(" << index << ")";
#endif
	switch(index.internalId() &0xF)
	{
		case lists:
			switch(index.row())
			{
			case 0:					return nodeHeader(index, role);
			case 1:					return beamHeader(index, role);
			case 2:					return loadHeader(index, role);
			case 3:					return blsHeader(index, role);
			case 4:					return clsHeader(index, role);
			}
		case node:					return nodeItem(index, role);
		case support:				return supportItem(index, role);
		case beam:					return beamItem(index, role);
		case hinge:					return hingeItem(index, role);
		case load:					return loadItem(index, role);
		case bls:					return blsItem(index, role);
		case cls:					return clsItem(index, role);
		case clsbls:				return clsblsItem(index, role);
		default:					return QVariant();
	}

}

bool treeModel::hasChildren(const QModelIndex & parent) const
{
#ifdef TREEVERBOSE
	qDebug() << "treeModel::hasChildren(" << parent << ")";
#endif

	if(!parent.isValid())
		return true;
	switch(parent.internalId() & 0xF)
	{
	case lists:						return true;
	case node:						return !(parent.row()==t->node_n());
	case beam:						return !(parent.row()==t->beam_n());
	case cls:						return !(parent.row()==t->cls_n());
	case support:					return false;
	case hinge:						return false;
	case load:						return false;
	case bls:						return false;
	case clsbls:					return false;
	default:						return false;
	}
}

Qt::ItemFlags treeModel::flags(const QModelIndex &index) const
{
#ifdef TREEVERBOSE
	qDebug() << "treeModel::flags(" << index << ")";
#endif

	switch(index.internalId() & 0xF)
	{
	case lists:						return Qt::ItemIsEnabled;
	case node:
	case beam:
	case support:
	case clsbls:
	case hinge:
	case bls:
		switch(index.column())
		{
		case 0:						return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		default:					return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
		}
	case cls:
		switch(index.column())
		{
		case 0:						return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		case 2:						return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
		default:				 	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
		}
	default:						return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

	}

}

bool treeModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	qDebug() << "treeModel::setData(" << index << ") = " << value.toString() << "role = " << role;
	switch(role){
	case Qt::EditRole:
		switch(index.internalId() & 0xF){
		case node:						return setNode(index, value);
		case beam:						return setBeam(index, value);
		case load:						return setLoad(index, value);
		case bls:						return setBLS(index, value);
		case cls:						return setCLS(index, value);
		case support:					return setSupport(index, value);
		case hinge:						return setHinge(index, value);
		case clsbls:					return setCLSBLS(index, value);
		default:						return false;
		}
	default:
		return false;
	}
}

treeModel::modelIds treeModel::parentId(treeModel::modelIds id)
{
	id &= 0xF;
	switch(id){
	case support:					return node;
	case cls:						return lists;
	case bls:						return lists;
	case node:						return lists;
	case beam:						return lists;
	case load:						return lists;
	case hinge:						return beam;
	case clsbls:					return cls;
	default:						return invalid;
	}
}

treeModel::modelIds treeModel::childId(treeModel::modelIds id, int row)
{
	id &= 0xF;
	switch(id){
	case lists:
		switch(row){
		case 0:						return node;
		case 1:						return beam;
		case 2:						return load;
		case 3:						return bls;
		case 4:						return cls;
		default:					return invalid;
		}
	case node:						return support | static_cast<modelId>(row<<4);
	case beam:						return hinge | static_cast<modelId>(row<<4);
	case cls:						return clsbls | static_cast<modelId>(row<<4);
	case root:						return lists;
	default:						return invalid;
	}
}
int treeModel::listindex(const QModelIndex &index)
{
	switch(index.internalId() & 0xF)
	{
	case node:						return 0;
	case beam:						return 1;
	case load:						return 2;
	case bls:						return 3;
	case cls:						return 4;
	case support:					return (index.internalId() & ~0xF)>>4;
	case hinge:						return (index.internalId() & ~0xF)>>4;
	case clsbls:					return (index.internalId() & ~0xF)>>4;
	default:						return 0;
	}
}

QVariant treeModel::nodeHeader(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("nodes"));
		case 1:						return QVariant(tr("x"));
		case 2:						return QVariant(tr("z"));
		case 3:						return QVariant(tr("phi"));
		case 4:						return QVariant(tr("angle"));
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::drawNode));
		default:					return QVariant();
		}
	case Qt::EditRole:				return QVariant();
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant(QSize(50, 24));
	case Qt::BackgroundRole:		return QVariant(HeaderBrush);
	case Qt::FontRole:				return QVariant(HeaderFont);
	case Qt::TextAlignmentRole:
		switch(index.column())
		{
		case 0:						return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
		default:					return QVariant(Qt::AlignCenter);
		}
	}
	return QVariant();
}

QVariant treeModel::beamHeader(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("beams"));
		case 1:						return QVariant(tr("from"));
		case 2:						return QVariant(tr("to"));
		case 3:						return QVariant(tr("material"));
		case 4:						return QVariant(tr("cross section"));
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::drawBeam));
		default:					return QVariant();
		}
	case Qt::EditRole:				return QVariant();
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant(QSize(50, 24));
	case Qt::BackgroundRole:		return QVariant(HeaderBrush);
	case Qt::FontRole:				return QVariant(HeaderFont);
	case Qt::TextAlignmentRole:
		switch(index.column())
		{
		case 0:						return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
		default:					return QVariant(Qt::AlignCenter);
		}
	}
	return QVariant();
}

QVariant treeModel::loadHeader(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("loads"));
		case 1:						return QVariant(tr("position"));
		case 2:						return QVariant(tr("x"));
		case 3:						return QVariant(tr("z"));
		case 4:						return QVariant(tr("phi"));
		case 5:						return QVariant(tr("set"));
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::drawNLoad));
		default:					return QVariant();
		}
	case Qt::EditRole:				return QVariant();
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case Qt::BackgroundRole:		return QVariant(HeaderBrush);
	case Qt::FontRole:				return QVariant(HeaderFont);
	case Qt::TextAlignmentRole:
		switch(index.column())
		{
		case 0:						return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
		default:					return QVariant(Qt::AlignCenter);
		}
	}
	return QVariant();
}

QVariant treeModel::blsHeader(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("load sets"));
		case 1:						return QVariant(tr("name"));
		case 2:						return QVariant(tr("color"));
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::BLS));
		default:					return QVariant();
		}
	case Qt::EditRole:				return QVariant();
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case Qt::BackgroundRole:		return QVariant(HeaderBrush);
	case Qt::FontRole:				return QVariant(HeaderFont);
	case Qt::TextAlignmentRole:
		switch(index.column())
		{
		case 0:						return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
		default:					return QVariant(Qt::AlignCenter);
		}
	}
	return QVariant();
}

QVariant treeModel::clsHeader(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("combined load sets"));
		case 1:						return QVariant(tr("name"));
		case 2:						return QVariant(tr("active"));
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::CLS));
		default:					return QVariant();
		}
	case Qt::EditRole:				return QVariant();
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case Qt::BackgroundRole:		return QVariant(HeaderBrush);
	case Qt::FontRole:				return QVariant(HeaderFont);
	case Qt::TextAlignmentRole:
		switch(index.column())
		{
		case 0:						return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
		default:					return QVariant(Qt::AlignCenter);
		}
	}
	return QVariant();

}

QVariant treeModel::nodeItem(const QModelIndex &index, int role) const
{
	int id = index.row();

	if(id==t->node_n())
		return newNodeItem(index, role);

	const ColinNode &n = t->node(id);
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString::number(id));
		case 1:						return QVariant(QString::number(n.x(), 'f', PRECISON)+"m");
		case 2:						return QVariant(QString::number(n.z(), 'f', PRECISON)+"m");
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::drawNode));
		default:					return QVariant();
		}
	case Qt::EditRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString::number(id));
		case 1:						return QVariant(QString::number(n.x(), 'f', PRECISON));
		case 2:						return QVariant(QString::number(n.z(), 'f', PRECISON));
		default:					return QVariant();
		}
	case Qt::ToolTipRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString(tr("<b>node #%1</b><br />")+
															tr("<b>x</b>=%2 m <br />")+
															tr("<b>z</b>=%3 m<br />")+
															tr("%4<br /><br />")+
															tr("press triangle to show support!"))
															.arg(index.row())
															.arg(n.x(), 0, 'f', PRECISON)
															.arg(n.z(), 0, 'f', PRECISON)
															.arg((n.hasbearing()?QString(tr("<b>support</b>: %1 %2 %3")
																						 .arg(n.bearing().x()?"X":"-")
																						 .arg(n.bearing().z()?"Z":"-")
																						 .arg(n.bearing().phi()?"P":"-")):"")));
		case 1:						return QVariant(QString(tr("<b>x</b>=%1 m<br />click to edit.")
															.arg(t->node(index.row()).x(), 0, 'f', PRECISON)));
		case 2:						return QVariant(QString(tr("<b>z</b>=%1 m<br />click to edit.")
															.arg(t->node(index.row()).z(), 0, 'f', PRECISON)));
		default:					return QVariant();
		}

	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	}
	return QVariant();
}

QVariant treeModel::beamItem(const QModelIndex &index, int role) const
{
	int id = index.row();

	if(id==t->beam_n())
		return newBeamItem(index, role);

	const ColinBeam &n = t->beam(id);
	switch(role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString::number(id));
		case 1:						return QVariant(QString::number(n.leftNodeI()));
		case 2:						return QVariant(QString::number(n.rightNodeI()));
		case 3:						return QVariant(n.Mat().name());
		case 4:						return QVariant(n.Profile().name());
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::drawBeam));
		default:					return QVariant();
		}
	case Qt::ToolTipRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString(tr("<b>beam #%1</b> (%2)--(%3)<br />")+
															tr("<b>lenght</b>: %4 m<br />")+
															tr("<b>angle</b>: %5 grad<br />")+
															tr("<b>material</b>: %6<br />")+
															tr("<b>cross section</b>: %7<br /><br />")+
															tr("press triangle to show hings!"))
													.arg(id)
													.arg(n.leftNodeI())
													.arg(n.rightNodeI())
													.arg(n.l(), 0, 'f', PRECISON)
													.arg(-n.angle()*ANGLEPREFIX, 0, 'f', PRECISON)
													.arg(n.Mat().name())
													.arg(n.Profile().name()));
		case 1:						return QVariant(QString(tr("<b>left node</b>: #%1 <br />(%2m, %3m)<br />")+
															tr("click to edit!"))
													.arg(n.leftNodeI())
													.arg(n.leftNode().x(), 0, 'f', PRECISON)
													.arg(n.leftNode().z(), 0, 'f', PRECISON));
		case 2:						return QVariant(QString(tr("<b>right node</b>: #%1 <br />(%2m, %3m)<br />")+
															tr("click to edit!"))
													.arg(n.rightNodeI())
													.arg(n.rightNode().x(), 0, 'f', PRECISON)
													.arg(n.rightNode().z(), 0, 'f', PRECISON));
		case 3:						return QVariant(QString(tr("<b>material</b>: %1 <br />")+
															tr("<b>E</b> = %2 %3<br />")+
															tr("<b>aT</b> = %4 %5<br /><br />")+
															tr("click to edit"))
													.arg(n.Mat().name())
													.arg(n.Mat().E()*EPREFIX).arg(UNIT.Eeh())
													.arg(n.Mat().alphaT()).arg("K"));
		case 4:						return QVariant(QString(tr("<b>cross section</b>: %1 <br />")+
															tr("<b>h</b> = %2 %3<br />")+
															tr("<b>A</b> = %4 %5<br />")+
															tr("<b>I</b> = %6 %7<br /><br />")+
															tr("click to edit"))
													.arg(n.Profile().name())
													.arg(n.Profile().h()*HPREFIX, 0, 'f', PRECISON).arg(UNIT.heh())
													.arg(n.Profile().A()*APREFIX, 0, 'f', PRECISON).arg(UNIT.Aeh())
													.arg(n.Profile().I()*IPREFIX, 0, 'f', PRECISON).arg(UNIT.Ieh()));
		default:					return QVariant();
		}

	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	}
	return QVariant();
}

QVariant treeModel::loadItem(const QModelIndex &index, int role) const
{
	int id = index.row();

	if(id==t->load_n())
		return newLoadItem(index, role);

	const ColinLoad &n = t->load(id);
	QString prefix;
	switch(n.typ()){
	case ColinLoad::nodeLoad:					prefix = UNIT.Peh();			break;
	case ColinLoad::moment:						prefix = UNIT.Meh();			break;
	case ColinLoad::increasingLinearLoad:
	case ColinLoad::decreasingLinearLoad:
	case ColinLoad::uniformlyDistibutedLoad:	prefix = UNIT.Peh()+"/m";		break;
	case ColinLoad::tempChange:
	case ColinLoad::tempDiffrence:				prefix = "K";					break;
	case ColinLoad::doubleLoadLeft:				prefix = "K";					break;
	case ColinLoad::doubleLoadRight:			prefix = "K";					break;
	}

	switch(role){
	case Qt::DisplayRole:
		switch(index.column()){
		case 0:						return QVariant(QString::number(id));
		case 1:						return QVariant(QString::number(n.at()));
		case 2:						return QVariant(QString::number(n.Px(), 'f', PRECISON)+prefix);
		case 3:						return QVariant(QString::number(n.Pz(), 'f', PRECISON)+prefix);
		case 4:						return QVariant(QString::number(n.M(), 'f', PRECISON)+prefix);
		case 5:						return QVariant(t->bls(n.set()).name());
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column()){
		case 0:						return QVariant(colinIcons::instance().icon(n.typ()));
		case 5:						return QVariant(colinIcons::instance().icon(t->bls(n.set()).color()));
		default:					return QVariant();
		}
	case Qt::EditRole:
		switch(index.column()){
		case 0:						return QVariant(loadTypeDescription(n.typ()));
		case 1:						return QVariant(QString::number(n.at()));
		case 2:						return QVariant(QString::number(n.Px(), 'f', PRECISON));
		case 3:						return QVariant(QString::number(n.Pz(), 'f', PRECISON));
		case 4:						return QVariant(QString::number(n.M(), 'f', PRECISON));
		case 5:						return QVariant(t->bls(n.set()).name());
		default:					return QVariant();
		}
	case Qt::ToolTipRole:
		switch(index.column())
		{
		case 0:
			switch(n.typ())
			{
			case ColinLoad::nodeLoad:
			case ColinLoad::increasingLinearLoad:
			case ColinLoad::decreasingLinearLoad:
			case ColinLoad::uniformlyDistibutedLoad:
									return QVariant(QString(tr("<b>%1</b>: #%2 <br />")+
															tr("<b>Px</b> = %3 %4<br />")+
															tr("<b>Pz</b> = %5 %6<br /><br />")+
															tr("click to edit"))
													.arg(loadItem(index, Qt::EditRole).toString())
													.arg(id)
													.arg(n.Px()*PPREFIX, 0, 'f', PRECISON).arg(UNIT.Peh()+(n.isOnBeam()?"/m":""))
													.arg(n.Pz()*PPREFIX, 0, 'f', PRECISON).arg(UNIT.Peh()+(n.isOnBeam()?"/m":"")));
			default:			return QVariant();
			}
		case 1:
			switch(n.typ())
			{
			case ColinLoad::nodeLoad:
			case ColinLoad::increasingLinearLoad:
			case ColinLoad::decreasingLinearLoad:
			case ColinLoad::uniformlyDistibutedLoad:
									return QVariant(QString(tr("<b>Px</b>: %1 %2<br />")+
															tr("click to edit!"))
													.arg(n.Px()*PPREFIX, 0, 'f', PRECISON).arg(UNIT.Peh()+(n.isOnBeam()?"/m":"")));
			case ColinLoad::tempChange:
									return QVariant(QString(tr("<b>DT</b>: %1 %2<br />")+
															tr("click to edit!"))
													.arg(n.Px(), 0, 'f', PRECISON).arg("K"));

			default:				return QVariant(QString(tr("unused fro this type of load")));
			}

		default:					return QVariant();
		}
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case Qt::UserRole:
		switch(index.column()){
		case 0:
			switch(n.typ()){
			case ColinLoad::nodeLoad:
			case ColinLoad::moment:
									return ColinStruct::NodalLoadTyp;
			case ColinLoad::uniformlyDistibutedLoad:
			case ColinLoad::decreasingLinearLoad:
			case ColinLoad::increasingLinearLoad:
									return ColinStruct::BeamLoadTyp;
			case ColinLoad::tempChange:
			case ColinLoad::tempDiffrence:
									return ColinStruct::TempLoadTyp;
			case ColinLoad::doubleLoadLeft:
			case ColinLoad::doubleLoadRight:
									return ColinStruct::DoubleLoadTyp;
			default:				return QVariant();
			}

		default:					return QVariant();
		}
	}
	return QVariant();
}

QVariant treeModel::blsItem(const QModelIndex &index, int role) const
{
	int id = index.row();

	if(id==t->bls_n())
		return newBLSItem(index, role);

	const ColinBLS &n = t->bls(id);
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString::number(id));
		case 1:						return QVariant(n.name());
		case 2:						return QVariant(n.color().name());
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::BLS));
		case 2:						return QVariant(colinIcons::instance().icon(n.color()));
		default:					return QVariant();
		}
	case Qt::EditRole:				return QVariant();
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case Qt::UserRole:
		switch(index.column())
		{
		case 2:						return QVariant(ColinStruct::BLScolor);
		default:					return QVariant();
		}
	}
	return QVariant();
}

QVariant treeModel::clsItem(const QModelIndex &index, int role) const
{
	int id = index.row();

	if(id==t->cls_n())
		return newCLSItem(index, role);

	const ColinCLS &n = t->cls(id);
	switch(role)
	{
	case Qt::EditRole:
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString::number(id));
		case 1:						return QVariant(n.name());
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::CLS));
		default:					return QVariant();
		}
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case Qt::CheckStateRole:
		switch(index.column()){
		case 2:						return t->activeCLS().contains(id);
		default:					return QVariant();
		}
	}
	return QVariant();
}

QVariant treeModel::supportItem(const QModelIndex &index, int role) const
{
	int id = index.parent().row();

	ColinSupport n;
	if(t->node(id).hasbearing())
		n = t->node(id).bearing();
	else
		n = ColinSupport();
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString::number(id));
		case 1:						return QVariant("x="+(n.x()?tr("locked"):n.xf()?QString::number(n.xf()*FPREFIX)+UNIT.Feh():tr("free")));
		case 2:						return QVariant("z="+(n.z()?tr("locked"):n.zf()?QString::number(n.zf()*FPREFIX)+UNIT.Feh():tr("free")));
		case 3:						return QVariant(QString(QChar(0x03C6))+"="+(n.phi()?tr("locked"):n.phif()?QString::number(n.phif()*FMPREFIX)+UNIT.FMeh():tr("free")));
		case 4:						return QVariant(QString::number(n.angle()*APREFIX, 'f', PRECISON)+"grad");
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(n.form()));
		default:					return QVariant();
		}
	case Qt::EditRole:
		switch(index.column())
		{
		case 1:						return QVariant(n.x()?tr("locked"):n.xf()?QString::number(n.xf()*FPREFIX):tr("free"));
		case 2:						return QVariant(n.z()?tr("locked"):n.zf()?QString::number(n.zf()*FPREFIX):tr("free"));
		case 3:						return QVariant(n.phi()?tr("locked"):n.phif()?QString::number(n.phif()*FMPREFIX):tr("free"));
		case 4:						return QVariant(QString::number(n.angle()*ANGLEPREFIX));
		}
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case 32:
		switch(index.column())
		{
		case 1:						return QVariant(ColinStruct::Bearing_h);
		case 2:						return QVariant(ColinStruct::Bearing_v);
		case 3:						return QVariant(ColinStruct::Bearing_m);
		default:					return QVariant();
		}
	}
	return QVariant();
}

QVariant treeModel::hingeItem(const QModelIndex &index, int role) const
{
	return QVariant();
}

QVariant treeModel::clsblsItem(const QModelIndex &index, int role) const
{

	int id = index.parent().row();

	if(index.row()>t->cls(id).count())
		return newCLSBLSItem(index, role);

	const ColinCLS &c = t->cls(id);

	switch(role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString::number(index.row()));
		case 1:						return QVariant(t->bls(c.bls(index.row())).name());
		case 2:						return QVariant(QString::number(c.bls(index.row()), 'f', PRECISON));
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::BLS));
		default:					return QVariant();
		}
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case 32:
		switch(index.column())
		{
		case 1:						return QVariant(ColinStruct::CLSBLS);
		default:					return QVariant();
		}
	}
	return QVariant();
}

QVariant treeModel::newNodeItem(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("add Node"));
		case 1:						return QVariant(newNodeBuffer[0]);
		case 2:						return QVariant(newNodeBuffer[1]);
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::addNode));
		default:					return QVariant();
		}
	case Qt::ToolTipRole:
		switch(index.column())
		{
		case 0:						return QVariant(QString(tr("enter values within this line to add new nodes to your system!")));
		case 1:						return QVariant(QString(tr("enter the x coordinate of the new node")));
		case 2:						return QVariant(QString(tr("enter the x coordinate of the new node")));
		default:					return QVariant();
		}
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	default:						return QVariant();
	}
}

QVariant treeModel::newBeamItem(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("add Beam"));
		case 1:						return QVariant(newBeamBuffer[0]);
		case 2:						return QVariant(newBeamBuffer[1]);
		case 3:						return QVariant(newBeamBuffer[2]);
		case 4:						return QVariant(newBeamBuffer[3]);
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::addBeam));
		default:					return QVariant();
		}
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	default:						return QVariant();
	}
}

QVariant treeModel::newLoadItem(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("add Load"));
		case 1:						return QVariant(newLoadBuffer[0]);
		case 2:						return QVariant(newLoadBuffer[1]);
		case 3:						return QVariant(newLoadBuffer[2]);
		case 4:						return QVariant(newLoadBuffer[3]);
		case 5:						return QVariant(newLoadBuffer[4]);
		default:					return QVariant();
		}
	case Qt::EditRole:
		switch(index.column())
		{
		case 0:						return QVariant(loadTypeDescription(newLoadFormBuffer));
		case 1:						return QVariant(newLoadBuffer[0]);
		case 2:						return QVariant(newLoadBuffer[1]);
		case 3:						return QVariant(newLoadBuffer[2]);
		case 4:						return QVariant(newLoadBuffer[3]);
		case 5:						return QVariant(newLoadBuffer[4]);
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().newIcon(Colin::addLoad, newLoadFormBuffer));
		default:					return QVariant();
		}
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case Qt::UserRole:
		switch(index.column())
		{
		case 0:						return QVariant(ColinStruct::LoadTyp);
		default:					return QVariant();
		}
	default:						return QVariant();
	}
}

QVariant treeModel::newBLSItem(const QModelIndex &index, int role) const
{
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case 0:						return QVariant(tr("add BLS"));
		case 1:						return QVariant(newBLSBuffer[0]);
		case 2:						return QVariant(newBLSBuffer[1]);
		default:					return QVariant();
		}
	case Qt::EditRole:
		switch(index.column())
		{
		case 0:						return QVariant();
		case 1:						return QVariant(newLoadBuffer[0]);
		case 2:						return QVariant(newLoadBuffer[1]);
		default:					return QVariant();
		}
	case Qt::DecorationRole:
		switch(index.column())
		{
		case 0:						return QVariant(colinIcons::instance().icon(Colin::addBLS));
		default:					return QVariant();
		}
	case Qt::ToolTipRole:			return QVariant();
	case Qt::StatusTipRole:			return QVariant();
	case Qt::WhatsThisRole:			return QVariant();
	case Qt::SizeHintRole:			return QVariant();
	case Qt::UserRole:				return QVariant();
	default:						return QVariant();
	}}

QVariant treeModel::newCLSItem(const QModelIndex &index, int role) const
{
	return QVariant();
}

QVariant treeModel::newCLSBLSItem(const QModelIndex &index, int role) const
{
	return QVariant();
}

bool treeModel::setNode(const QModelIndex & index, const QVariant & value)
{
	if(index.row()==t->node_n())
		return addNode(index, value);

	bool ok;
	double val;
	switch(index.column()){
		case 1:
			val = value.toString().toDouble(&ok);
			if(ok)			t->setX(index.row(), val);
			return ok;
		case 2:
			val = value.toString().toDouble(&ok);
			if(ok)			t->setZ(index.row(), val);
			return ok;
		default:
			return false;
	}
}

bool treeModel::setBeam(const QModelIndex & index, const QVariant & value)
{
	if(index.row()==t->beam_n())
		return addBeam(index, value);

	bool ok;
	double val;
	switch(index.column()){
		case 1:
			val = value.toString().toInt(&ok);
			if(!ok)			t->setLeft(index.row(), val);
			return ok;
		case 2:
			val = value.toString().toInt(&ok);
			if(ok)			t->setRight(index.row(), val);
			return ok;
		case 3:
			val = value.toString().toInt(&ok);
			if(ok)			t->setMat(index.row(), val);
			return ok;
		case 4:
			val = value.toString().toInt(&ok);
			if(ok)			t->setProfile(index.row(), val);
			return ok;
		default:
			return false;
	}
}

bool treeModel::setLoad(const QModelIndex & index, const QVariant & value)
{
	if(index.row()==t->load_n())
		return addLoad(index, value);

	bool ok;
	double val;
	int ival;
	switch(index.column())
	{
	case 0:
		t->setLoadTyp(index.row(), static_cast<ColinLoad::form>(loadType(value.toString())));
		return true;
	case 1:
		ival = value.toInt(&ok);
		if(ok)
			t->setPos(index.row(), ival);
		return ok;
	case 2:
		val = value.toDouble(&ok);
		if(ok)
			t->setPx(index.row(), ival);
		return ok;
	case 3:
		val = value.toDouble(&ok);
		if(ok)
			t->setPz(index.row(), ival);
		return ok;
	case 4:
		val = value.toDouble(&ok);
		if(ok)
			t->setM(index.row(), ival);
		return ok;
	case 5:
		t->setLoadSet(index.row(), t->getBLSIDbyName(value.toString()));
		return true;
	default:
		return false;
	}

}

bool treeModel::setBLS(const QModelIndex & index, const QVariant & value)
{
	if(index.row()==t->bls_n())
		return addBLS(index, value);

	switch(index.column())
	{
	case 1:
		t->setBLSName(index.row(), value.toString());
		return true;
	case 2:
		t->setBLSColor(index.row(), QColor(value.toString()));
		return true;
	default:
		return false;
	}
}

bool treeModel::setCLS(const QModelIndex & index, const QVariant & value)
{
	if(index.row()==t->cls_n())
		return addCLS(index, value);

	switch(index.column())
	{
	case 1:
		t->setCLSName(index.row(), value.toString());
		return true;
	default:
		return false;
	}
}

bool treeModel::setSupport(const QModelIndex & index, const QVariant & value)
{
	return true;
}

bool treeModel::setHinge(const QModelIndex & index, const QVariant & value)
{
	return true;

}

bool treeModel::setCLSBLS(const QModelIndex & index, const QVariant & value)
{
	return true;

}

bool treeModel::addNode(const QModelIndex & index, const QVariant & value)
{
	qDebug() << "treeModel::addNode, new value for " << index.column() << " = " << value.toString();
	newNodeBuffer[index.column()-1]=value.toString();
	bool ok;
	double x = newNodeBuffer[0].toDouble(&ok);
	if(!ok)
		return true;
	double y = newNodeBuffer[1].toDouble(&ok);
	if(!ok)
		return true;

	t->addNode(x, y);

	newNodeBuffer[0] = tr("x");
	newNodeBuffer[1] = tr("z");

	return true;
}

bool treeModel::addBeam(const QModelIndex & index, const QVariant & value)
{
	qDebug() << "treeModel::addBeam, new value for " << index.column() << " = " << value.toString();
	newBeamBuffer[index.column()-1]=value.toString();
	bool ok;
	int l = newBeamBuffer[0].toInt(&ok);
	if(!ok)
		return true;
	int r = newBeamBuffer[1].toInt(&ok);
	if(!ok)
		return true;

	int matId = LIB.IndexFromMatByName(newBeamBuffer[2]);

	if(matId < 0){
		if(t->beam_n()){
			matId = t->beam(t->beam_n()-1).MatI();
		}
		else{
			matId = 0;
		}
	}

	int csId = LIB.IndexFromProfileByName(newBeamBuffer[3]);

	if(csId < 0){
		if(t->beam_n()){
			csId = t->beam(t->beam_n()-1).ProfileI();
		}
		else{
			csId = 0;
		}
	}

	t->addBeam(	ColinBeam(t, l, r, matId, csId));


	newBeamBuffer[0]=tr("from");
	newBeamBuffer[1]=tr("to");
	newBeamBuffer[2]=tr("material");
	newBeamBuffer[3]=tr("cross section");

	return true;
}

bool treeModel::addLoad(const QModelIndex & index, const QVariant & value)
{
	qDebug() << "treeModel::addLoad, new value for " << index.column() << " = " << value.toString();
	if(index.column() == 0){
		newLoadFormBuffer = loadType(value.toString());
		setLoadBuffer(newLoadBuffer, newLoadFormBuffer);
	}
	else
		newLoadBuffer[index.column()-1]=value.toString();
	bool ok;
	int pos = newLoadBuffer[0].toInt(&ok);
	if(!ok)
		return true;

	double px, pz, m, set;

	switch(newLoadFormBuffer)
	{
	case ColinLoad::nodeLoad:
	case ColinLoad::increasingLinearLoad:
	case ColinLoad::decreasingLinearLoad:
	case ColinLoad::uniformlyDistibutedLoad:
		px = newBeamBuffer[1].toDouble(&ok);
		if(!ok)	return true;
		pz = newBeamBuffer[2].toDouble(&ok);
		if(!ok) return true;
		set = t->getBLSIDbyName(newBeamBuffer[3]);
		t->addLoad(ColinLoad(static_cast<ColinLoad::form>(newLoadFormBuffer), pos, px, pz, 0, set));
		setLoadBuffer(newLoadBuffer, newLoadFormBuffer);
		return true;
	case ColinLoad::moment:
		m = newBeamBuffer[3].toDouble(&ok);
		if(!ok) return true;
		set = t->getBLSIDbyName(newBeamBuffer[3]);
		t->addLoad(ColinLoad(static_cast<ColinLoad::form>(newLoadFormBuffer), pos, 0, 0, m, set));
		setLoadBuffer(newLoadBuffer, newLoadFormBuffer);
		return true;
	case ColinLoad::tempChange:
		px = newBeamBuffer[1].toDouble(&ok);
		if(!ok) return true;
		set = t->getBLSIDbyName(newBeamBuffer[3]);
		t->addLoad(ColinLoad(static_cast<ColinLoad::form>(newLoadFormBuffer), pos, px, 0, 0, set));
		setLoadBuffer(newLoadBuffer, newLoadFormBuffer);
		return true;
	case ColinLoad::tempDiffrence:
		m = newBeamBuffer[2].toDouble(&ok);
		if(!ok) return true;
		set = t->getBLSIDbyName(newBeamBuffer[3]);
		t->addLoad(ColinLoad(static_cast<ColinLoad::form>(newLoadFormBuffer), pos, 0, 0, m, set));
		setLoadBuffer(newLoadBuffer, newLoadFormBuffer);
		return true;
	case ColinLoad::doubleLoadLeft:
	case ColinLoad::doubleLoadRight:
		px = newBeamBuffer[1].toDouble(&ok);
		if(!ok)	return true;
		pz = newBeamBuffer[2].toDouble(&ok);
		if(!ok) return true;
		m = newBeamBuffer[3].toDouble(&ok);
		if(!ok) return true;
		set = t->getBLSIDbyName(newBeamBuffer[3]);
		t->addLoad(ColinLoad(static_cast<ColinLoad::form>(newLoadFormBuffer), pos, px, pz, m, set));
		setLoadBuffer(newLoadBuffer, newLoadFormBuffer);
		return true;
	default:
		return true;
	}



	return true;
}

bool treeModel::addBLS(const QModelIndex & index, const QVariant & value)
{
	return true;
}

bool treeModel::addCLS(const QModelIndex & index, const QVariant & value)
{
	return true;
}

bool treeModel::addCLSBLS(const QModelIndex & index, const QVariant & value)
{
	return true;
}

QString treeModel::loadTypeDescription(int loadtype) const
{
	switch(loadtype){
	case ColinLoad::nodeLoad:
							return tr("nodal load");
	case ColinLoad::moment:
							return tr("moment");
	case ColinLoad::uniformlyDistibutedLoad:
							return tr("uniform beam load");
	case ColinLoad::decreasingLinearLoad:
							return tr("decreasing beam load");
	case ColinLoad::increasingLinearLoad:
							return tr("increasing beam load");
	case ColinLoad::tempChange:
							return tr("temperature change");
	case ColinLoad::tempDiffrence:
							return tr("temperature difference");
	case ColinLoad::doubleLoadLeft:
							return tr("double load right side");
	case ColinLoad::doubleLoadRight:
							return tr("double load left side");
	default:
							return "";
	}
}

void treeModel::setLoadBuffer(QString* buffer, int loadtype) const
{
	buffer[0] = tr("position");
	buffer[4] = tr("set");
	switch(loadtype){
	case ColinLoad::nodeLoad:
	case ColinLoad::uniformlyDistibutedLoad:
	case ColinLoad::decreasingLinearLoad:
	case ColinLoad::increasingLinearLoad:
		buffer[1] = tr("Px");
		buffer[2] = tr("Pz");
		buffer[3] = tr("-");
		return;
	case ColinLoad::moment:
		buffer[1] = tr("-");
		buffer[2] = tr("-");
		buffer[3] = tr("M");
		return;
	case ColinLoad::tempChange:
		buffer[1] = tr("DT");
		buffer[2] = tr("-");
		buffer[3] = tr("-");
		return;
	case ColinLoad::tempDiffrence:
		buffer[1] = tr("-");
		buffer[2] = tr("-");
		buffer[3] = tr("DT");
		return;
	case ColinLoad::doubleLoadLeft:
	case ColinLoad::doubleLoadRight:
		buffer[1] = tr("Px");
		buffer[2] = tr("Pz");
		buffer[3] = tr("M");
		return;
	}
}

int treeModel::loadType(const QString &description) const
{
	//slower but less problems to change:
	for(int i=0; i<10; i++){
		if(loadTypeDescription(i)==description)
		{
			return i;
		}
	}
	return 0;
}

void treeModel::setStruct(ColinStruct *tw)
{
	emit this->reset();
	if(t)
	{
		t->disconnect(this,				SLOT(addedBeam(int)));
		t->disconnect(this,				SLOT(addedLoad(int)));
		t->disconnect(this,				SLOT(addedNode(int)));
		t->disconnect(this,				SLOT(addedBLS(int)));
		t->disconnect(this,				SLOT(addedCLS(int)));
		t->disconnect(this,				SLOT(erasedBeam(int)));
		t->disconnect(this,				SLOT(erasedLoad(int)));
		t->disconnect(this,				SLOT(erasedNode(int)));
		t->disconnect(this,				SLOT(erasedBLS(int)));
		t->disconnect(this,				SLOT(erasedCLS(int)));
		t->disconnect(this,				SLOT(changedBeam(int)));
		t->disconnect(this,				SLOT(changedLoad(int)));
		t->disconnect(this,				SLOT(changedNode(int)));
		t->disconnect(this,				SLOT(changedBLS(int)));
		t->disconnect(this,				SLOT(changedCLS(int)));

	}
	t = tw;

	qDebug() << "setStruct(...)";

	if(t != 0)
	{
		connect(t,						SIGNAL(addedBeam(int)),
				this,					SLOT(addedBeam(int)));

		connect(t,						SIGNAL(addedLoad(int)),
				this,					SLOT(addedLoad(int)));

		connect(t,						SIGNAL(addedNode(int)),
				this,					SLOT(addedNode(int)));

		connect(t,						SIGNAL(addedBLS(int)),
				this,					SLOT(addedBLS(int)));

		connect(t,						SIGNAL(addedCLS(int)),
				this,					SLOT(addedCLS(int)));


		connect(t,						SIGNAL(changedBeam(int)),
				this,					SLOT(changedBeam(int)));

		connect(t,						SIGNAL(changedNode(int)),
				this,					SLOT(changedNode(int)));

		connect(t,						SIGNAL(changedLoad(int)),
				this,					SLOT(changedLoad(int)));

		connect(t,						SIGNAL(changedBLS(int)),
				this,					SLOT(changedBLS(int)));

		connect(t,						SIGNAL(changedCLS(int)),
				this,					SLOT(changedCLS(int)));


		connect(t,						SIGNAL(erasedBeam(int)),
				this,					SLOT(erasedBeam(int)));

		connect(t,						SIGNAL(erasedNode(int)),
				this,					SLOT(erasedNode(int)));

		connect(t,						SIGNAL(erasedLoad(int)),
				this,					SLOT(erasedLoad(int)));

		connect(t,						SIGNAL(erasedBLS(int)),
				this,					SLOT(erasedBLS(int)));

		connect(t,						SIGNAL(erasedCLS(int)),
				this,					SLOT(erasedCLS(int)));
	}

	emit dataChanged(index(0,0, QModelIndex()), index(0,0, QModelIndex()));
}

void treeModel::changedNode(const int &i)
{
	qDebug() << "changed Node x= "<< t->node(i).x();
	emit dataChanged(createIndex(i, 0, node), createIndex(i, 6, node));
}

void treeModel::changedBeam(const int &i)
{
	emit dataChanged(createIndex(i, 0, beam), createIndex(i, 6, beam));
}

void treeModel::changedLoad(const int &i)
{
	emit dataChanged(createIndex(i, 0, load), createIndex(i, 6, load));
}

void treeModel::changedBLS(const int &i)
{
	emit dataChanged(createIndex(i, 0, bls), createIndex(i, 6, bls));
}

void treeModel::changedCLS(const int &i)
{
	emit dataChanged(createIndex(i, 0, cls), createIndex(i, 6, cls));
}

void treeModel::erasedNode(const int &i)
{
	emit beginRemoveRows(createIndex(0, 0, lists), i, i);
	emit dataChanged(createIndex(0, 0, lists), createIndex(0, 6, lists));
	emit endRemoveRows();

}

void treeModel::erasedBeam(const int &i)
{
	emit beginRemoveRows(createIndex(1, 0, lists), i, i);
	emit dataChanged(createIndex(1, 0, lists), createIndex(1, 6, lists));
	emit endRemoveRows();
}

void treeModel::erasedLoad(const int &i)
{
	emit beginRemoveRows(createIndex(2, 0, lists), i, i);
	emit dataChanged(createIndex(2, 0, lists), createIndex(2, 6, lists));
	emit endRemoveRows();
}

void treeModel::erasedBLS(const int &i)
{
	emit beginRemoveRows(createIndex(3, 0, lists), i, i);
	emit dataChanged(createIndex(3, 0, lists), createIndex(3, 6, lists));
	emit endRemoveRows();
}

void treeModel::erasedCLS(const int &i)
{
	emit beginRemoveRows(createIndex(4, 0, lists), i, i);
	emit dataChanged(createIndex(4, 0, lists), createIndex(4, 6, lists));
	emit endRemoveRows();
}

void treeModel::addedNode(const int &i)
{
	emit beginInsertRows(createIndex(0, 0, lists), i, i);
	emit dataChanged(createIndex(0, 0, lists), createIndex(0, 0, lists));
	emit endInsertRows();
}

void treeModel::addedBeam(const int &i)
{
	emit beginInsertRows(createIndex(1, 0, lists), i, i);
	emit dataChanged(createIndex(1, 0, lists), createIndex(1, 0, lists));
	emit endInsertRows();
}

void treeModel::addedLoad(const int &i)
{
	emit beginInsertRows(createIndex(2, 0, lists), i, i);
	emit dataChanged(createIndex(2, 0, lists), createIndex(2, 0, lists));
	emit endInsertRows();
}

void treeModel::addedBLS(const int &i)
{
	emit beginInsertRows(createIndex(3, 0, lists), i, i);
	emit dataChanged(createIndex(3, 0, lists), createIndex(3, 0, lists));
	emit endInsertRows();
}

void treeModel::addedCLS(const int &i)
{
	emit beginInsertRows(createIndex(4, 0, lists), i, i);
	emit dataChanged(createIndex(4, 0, lists), createIndex(4, 0, lists));
	emit endInsertRows();
}
