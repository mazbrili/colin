/***********************************************************
 * $Id$
 *
 * Colin
 *
 * Copyright (C) 2011 Matthias Rauter (matthias.rauter@student.uibk.ac.at)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 *
 * http://www.gnu.org/licenses/.
 *
 * Author: Matthias Rauter
 *
 ***********************************************************/

#include "xmlreader.h"
XmlReader::XmlReader(ColinStruct *t)
{
    if(t == NULL)
        tw = new ColinStruct();
    else
        tw = t;
}

bool XmlReader::read(QIODevice *device)
{
    setDevice(device);
    if(readNextStartElement())
        {
            if(name() == "ColinStruct" && attributes().value("version") == "0.01")
                readTw();
            else
                raiseError(QObject::tr("can't read file!"));
        }
    return !error();
}

bool XmlReader::read(const QByteArray &array)
{
#ifndef QT_NO_DEBUG
   QTextStream debugS(stdout);
   debugS << "readbing byteArray:" << endl;
   debugS << array << endl << endl;
#endif
    addData(array);
    if(readNextStartElement())
        {
            QPointF translation;
            if(name() == "ColinClipboard" && attributes().value("version") == "0.01")
            {
                translation.setX(attributes().value("translationX").toString().toDouble());
                translation.setY(attributes().value("translationZ").toString().toDouble());
                readTw();
                for(int i=0; i<tw->node_n(); i++)
                {
                    tw->nodes[i].setX(tw->node(i).x()-translation.x());
                    tw->nodes[i].setZ(tw->node(i).z()-translation.y());
                }
            }
            else if(name() == "ColinClipboardLoad" && attributes().value("version") == "0.01")
            {
                ColinLoad l;
                l.setTyp(static_cast<ColinLoad::form>(attributes().value("type").toString().toInt()));
                l.setBeam(0);
                l.setPx(attributes().value("Px").toString().toDouble());
                l.setPz(attributes().value("Pz").toString().toDouble());
                l.setM(attributes().value("M").toString().toDouble());
				if(attributes().value("set").toString()!=QString("none")){
					tw->CommandInsertBLS(tw->bls_n(), ColinBLS(attributes().value("set").toString()));
					l.setSet(tw->bls_n()-1);
				}
                tw->CommandInsertLoad(tw->load_n(), l);
            }
            else
                raiseError(QObject::tr("can't read file!"));
        }
    return !error();
}

void XmlReader::readTw()
{
    Q_ASSERT(isStartElement() && (name() == "ColinStruct" || name() == "ColinClipboard"));

    while(readNextStartElement())
    {


        if(name() == "node")
            readNode();
        else if(name() == "beam")
            readBeam();
        else if(name() == "load")
            readLoad();
        else if(name() == "material")
            readMat();
        else if(name() == "profile")
            readProfile();
		else if(name() == "bls")
			readBLS();
		else if(name() == "cls")
			readCLS();
        else
            skipCurrentElement();
    }

}

void XmlReader::readNode()
{
    Q_ASSERT(isStartElement() && name() =="node");

    ColinNode n(attributes().value("X").toString().toDouble(),
               attributes().value("Z").toString().toDouble());
    while(readNextStartElement())
    {
        if(name() == "bearing")
        {
            ColinSupport b = readBearing();
            n.setBearing(b);
        }
        else
            skipCurrentElement();
    }
    tw->CommandInsertNode(tw->node_n(), n);
}

void XmlReader::readBeam()
{
    Q_ASSERT(isStartElement() && name() == "beam");

    ColinBeam s(tw);
    s.setLeftNode(attributes().value("from").toString().toInt());
    s.setRightNode(attributes().value("to").toString().toInt());
    s.setMat(materialMap.value(attributes().value("material").toString()));
    s.setQs(profileMap.value(attributes().value("profile").toString()));

    while(readNextStartElement())
    {
        if(name() == "joints")
        {
            for(int i=0; i<6; i++)
            {
                if(attributes().value(QString("j%1").arg(i)).toString().toInt() == 1)
                    s.setJoint(i, true);
                else
                    s.setJoint(i, false);
            }
        }
        else if(name() == "springs")
        {
            for(int i=0; i<6; i++)
            {
                if(attributes().value(QString("s%1").arg(i)).toString().toDouble() != 0)
                s.setSpring(i, attributes().value(QString("s%1").arg(i)).toString().toDouble());
            }
        }
        skipCurrentElement();
    }

    tw->CommandInsertBeam(tw->beam_n(), s);
}

void XmlReader::readLoad()
{
    Q_ASSERT(isStartElement() && name() == "load");

    ColinLoad l;
    l.setTyp(static_cast<ColinLoad::form>(attributes().value("type").toString().toInt()));
    l.setBeam(attributes().value("position").toString().toInt());
    l.setPx(attributes().value("Px").toString().toDouble());
    l.setPz(attributes().value("Pz").toString().toDouble());
    l.setM(attributes().value("M").toString().toDouble());
	l.setSet(tw->getBLSIDbyName(attributes().value("set").toString()));
    tw->CommandInsertLoad(tw->load_n(), l);
    skipCurrentElement();
}

ColinSupport XmlReader::readBearing()
{
    Q_ASSERT(isStartElement() && name() == "bearing");

    ColinSupport b;

    b.setForm(static_cast<ColinSupport::bearings>(
            attributes().value("form").toString().toInt()));
    b.setAngle(attributes().value("angle").toString().toDouble());
    while(readNextStartElement())
    {
        if(name() == "springs")
        {
            b.setCx(attributes().value("cX").toString().toDouble());
            b.setCz(attributes().value("cZ").toString().toDouble());
            b.setCphi(attributes().value("cPhi").toString().toDouble());
        }
        else
            skipCurrentElement();
    }
	skipCurrentElement();
	return b;

}

void XmlReader::readBLS()
{
	Q_ASSERT(isStartElement() && name() == "bls");

	ColinBLS b;

	b.setName(attributes().value("name").toString());
	tw->CommandInsertBLS(tw->bls_n(), b);
	skipCurrentElement();
}

void XmlReader::readCLS()
{
	Q_ASSERT(isStartElement() && name() == "cls");

	ColinCLS c;

	c.setName(attributes().value("name").toString());
	while(readNextStartElement())
	{
		if(name()=="lschild")
		{
			c.addBLS(tw->getBLSIDbyName(attributes().value("blsName").toString()),
					 attributes().value("factor").toString().toDouble());
		}
		skipCurrentElement();
	}
	tw->CommandInsertCLS(tw->cls_n(), c);
}

void XmlReader::readMat()
{
    Q_ASSERT(isStartElement() && name() == "material");

    ColinMaterial m;
    m.setName(attributes().value("name").toString());
    m.setE(attributes().value("E").toString().toDouble());
//    m.setV(attributes().value("v").toString().toDouble());
//    m.setRoh(attributes().value("roh").toString().toDouble());
    m.setAlphaT(attributes().value("alphaT").toString().toDouble());

    //test for existing material and for existing name
    QString name = m.name();
    for(int i=1; i< 100000; i++)
    {
        int index = LIB.IndexFromMatByName(m.name());
        if(index > -1)
        {
            if(m == LIB.mat(index))
            {
                skipCurrentElement();
                materialMap.insert(name, index);
                return;
            }
            else
            {
                m.setName(name + "("+ QString::number(i)+ ")");
            }
        }
        else
        {
            break;
        }
    }
    LIB.addMat(m);
    materialMap.insert(m.name(), LIB.mats_n()-1);
    skipCurrentElement();
}

void XmlReader::readProfile()
{
    Q_ASSERT(isStartElement() && name() == "profile");

    ColinCrossSection p;
    p.setName(attributes().value("name").toString());
    p.setA(attributes().value("A").toString().toDouble());
    p.setI(attributes().value("I").toString().toDouble());
    p.setH(attributes().value("h").toString().toDouble());
//    p.setAq(attributes().value("Aq").toString().toDouble());
    QString name = p.name();
    for(int i=1; i < 100000; i++)
    {
        int index = LIB.IndexFromProfileByName(p.name());
        if(index > -1)
        {
            if(p == LIB.Profile(index))
            {
                skipCurrentElement();
                profileMap.insert(name, index);
                return;
            }
            else
            {
                p.setName(name + "("+ QString::number(i)+ ")");
            }
        }
        else
        {
            break;
        }
    }
    LIB.addProfile(p);
    profileMap.insert(p.name(), LIB.profiles_n()-1);
    skipCurrentElement();
}

QString XmlReader::errors() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(errorString())
            .arg(lineNumber())
            .arg(columnNumber());
}
