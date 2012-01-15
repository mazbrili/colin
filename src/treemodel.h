#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QtCore/QAbstractItemModel>

class ColinStruct;
class ColinNode;
class ColinBeam;
class ColinLoad;
class ColinSupport;
class ColinBLS;
class ColinCLS;

class treeModel : public QAbstractItemModel
{
    Q_OBJECT
public:

	enum modelId
	{
		lists		= 0x1,
		node		= 0x2,
		load		= 0x3,
		beam		= 0x4,
		bls			= 0x5,
		cls			= 0x6,
		support		= 0xA,
		hinge		= 0xB,
		clsbls		= 0xC,
		invalid		= 0xD,
		root		= 0xF // == 0b1111
	};

	Q_DECLARE_FLAGS(modelIds, modelId);


    explicit treeModel(QObject *parent = 0);

	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex & index) const;
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	int columnCount(const QModelIndex & parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);




	static QString treeNavigation();

	static int listindex(const QModelIndex &index);
	static inline treeModel::modelIds Id(int id){return modelIds(id&root);}

signals:

public slots:
	void setStruct(ColinStruct *tw);
	void changedNode(const int &i);
	void changedBeam(const int &i);
	void changedLoad(const int &i);
	void changedBLS(const int &i);
	void changedCLS(const int &i);

	void erasedNode(const int &i);
	void erasedBeam(const int &i);
	void erasedLoad(const int &i);
	void erasedBLS(const int &i);
	void erasedCLS(const int &i);

	void addedNode(const int &i);
	void addedBeam(const int &i);
	void addedLoad(const int &i);
	void addedBLS(const int &i);
	void addedCLS(const int &i);

private:
	ColinStruct *t;
	static treeModel::modelIds parentId(treeModel::modelIds id);
	static treeModel::modelIds childId(treeModel::modelIds id, int row);
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	//0
	QVariant nodeHeader(const QModelIndex &index, int role) const;
	QVariant beamHeader(const QModelIndex &index, int role) const;
	QVariant loadHeader(const QModelIndex &index, int role) const;
	QVariant blsHeader(const QModelIndex &index, int role) const;
	QVariant clsHeader(const QModelIndex &index, int role) const;
	//1
	QVariant nodeItem(const QModelIndex &index, int role) const;
	QVariant beamItem(const QModelIndex &index, int role) const;
	QVariant loadItem(const QModelIndex &index, int role) const;
	QVariant blsItem(const QModelIndex &index, int role) const;
	QVariant clsItem(const QModelIndex &index, int role) const;
	//2
	QVariant supportItem(const QModelIndex &index, int role) const;
	QVariant hingeItem(const QModelIndex &index, int role) const;
	QVariant clsblsItem(const QModelIndex &index, int role) const;
	//creationPatterns
	QVariant newNodeItem(const QModelIndex &index, int role) const;
	QVariant newBeamItem(const QModelIndex &index, int role) const;
	QVariant newLoadItem(const QModelIndex &index, int role) const;
	QVariant newBLSItem(const QModelIndex &index, int role) const;
	QVariant newCLSItem(const QModelIndex &index, int role) const;
	QVariant newCLSBLSItem(const QModelIndex &index, int role) const;

	bool setNode(const QModelIndex & index, const QVariant & value);
	bool setBeam(const QModelIndex & index, const QVariant & value);
	bool setLoad(const QModelIndex & index, const QVariant & value);
	bool setBLS(const QModelIndex & index, const QVariant & value);
	bool setCLS(const QModelIndex & index, const QVariant & value);
	bool setSupport(const QModelIndex & index, const QVariant & value);
	bool setHinge(const QModelIndex & index, const QVariant & value);
	bool setCLSBLS(const QModelIndex & index, const QVariant & value);

	bool addNode(const QModelIndex & index, const QVariant & value);
	bool addBeam(const QModelIndex & index, const QVariant & value);
	bool addLoad(const QModelIndex & index, const QVariant & value);
	bool addBLS(const QModelIndex & index, const QVariant & value);
	bool addCLS(const QModelIndex & index, const QVariant & value);
	bool addCLSBLS(const QModelIndex & index, const QVariant & value);

	QBrush headerDecoration(const QModelIndex &index) const;


	QString loadTypeDescription(int loadtype) const;
	int loadType(const QString &description) const;
	void setLoadBuffer(QString* buffer, int loadtype) const;

	QString newNodeBuffer[2];
	QString newBeamBuffer[4];
	QString newLoadBuffer[6];
	QString newBLSBuffer[2];
	QString newCLSBuffer[2];
	int newLoadFormBuffer;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(treeModel::modelIds);
#endif // TREEMODEL_H
