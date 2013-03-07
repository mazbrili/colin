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

#include "xmlwriter.h"

XmlWriter::XmlWriter(QIODevice *device):
        QXmlStreamWriter(device)
{
}

XmlWriter::XmlWriter(QByteArray *array) :
        QXmlStreamWriter(array)
{
}

void XmlWriter::writeLoad(const ColinStruct &tw, const int &nr)
{
    const ColinLoad &l = tw.load(nr);
    setAutoFormatting(true);
    writeStartDocument();
    writeStartElement("ColinClipboardLoad");
    writeAttribute("version", "0.01");
    writeAttribute("type", QString("%1").arg(l.typ()));
    writeAttribute("position", QString("%1").arg(l.at()));
    writeAttribute("Px", QString("%1").arg(l.Px()));
    writeAttribute("Pz", QString("%1").arg(l.Pz()));
    writeAttribute("M", QString("%1").arg(l.M()));
	if(l.set()>=0)
		writeAttribute("set", tw.bls(l.set()).name());
	else
		writeAttribute("set", "none");
	writeEndElement();
    writeEndDocument();//ColinClipboardLoad



}

void XmlWriter::writeSelection(const ColinStruct &tw, QPointF translation)
{
    setAutoFormatting(true);
    writeStartDocument();

    writeStartElement("ColinClipboard");
    writeAttribute("version", "0.01");

    writeAttribute("translationX", QString::number(translation.x()));
    writeAttribute("translationZ", QString::number(translation.y()));
    writeRequredLibEntries(tw);

    QMap<int, int> nodes;
	QMap<int, int> beams;
	QSet<int> bls;
    int beamCount = 0;
    int nodeCount = 0;



    for(int i=0; i<tw.node_n(); i++)
    {
        if(tw.node(i).isSelected())
        {
            writeNode(tw.node(i));
            nodes.insert(i, nodeCount++);
        }
        else if(tw.node(i).hasbearing())
        {
            if(tw.node(i).bearing().isSelected())
            {
                writeNode(tw.node(i));
                nodes.insert(i, nodeCount++);
            }
        }
    }

    for(int i=0; i<tw.beam_n(); i++)
    {
        if(tw.beam(i).isSelected())
        {
            if(!nodes.contains(tw.beam(i).leftNodeI()))
            {
                writeNode(tw.beam(i).leftNode());
                nodes.insert(tw.beam(i).leftNodeI(), nodeCount++);
            }
            if(!nodes.contains(tw.beam(i).rightNodeI()))
            {
                writeNode(tw.beam(i).rightNode());
                nodes.insert(tw.beam(i).rightNodeI(), nodeCount++);
            }

            ColinBeam b = tw.beam(i);
            b.setLeftNode(nodes.value(tw.beam(i).leftNodeI(), -1));
            b.setRightNode(nodes.value(tw.beam(i).rightNodeI(), -1));

            Q_ASSERT(b.leftNodeI() != -1);
            Q_ASSERT(b.rightNodeI() != -1);

            writeBeam(b);
            beams.insert(i, beamCount++);
        }
    }
    for(int i=0; i<tw.load_n(); i++)
    {
        if(tw.load(i).isSelected())
        {
            if(tw.load(i).typ() == ColinLoad::decreasingLinearLoad    ||
               tw.load(i).typ() == ColinLoad::increasingLinearLoad    ||
               tw.load(i).typ() == ColinLoad::uniformlyDistibutedLoad ||
               tw.load(i).typ() == ColinLoad::tempChange              ||
               tw.load(i).typ() == ColinLoad::tempDiffrence           )
            {
				if(tw.load(i).set()>-1 && !bls.contains(tw.load(i).set()))
				{
					writeBLS(tw.bls(tw.load(i).set()));
					bls.insert(tw.load(i).set());
				}
                if(!beams.contains(tw.load(i).at()))
		{
		    if(!nodes.contains(tw.beam(tw.load(i).at()).leftNodeI()))
		    {
			writeNode(tw.beam(tw.load(i).at()).leftNode());
			nodes.insert(tw.beam(tw.load(i).at()).leftNodeI(), nodeCount++);
		    }
		    if(!nodes.contains(tw.beam(tw.load(i).at()).rightNodeI()))
		    {
			writeNode(tw.beam(tw.load(i).at()).rightNode());
			nodes.insert(tw.beam(tw.load(i).at()).rightNodeI(), nodeCount++);
		    }

		    ColinBeam b = tw.beam(tw.load(i).at());
		    b.setLeftNode(nodes.value(tw.beam(tw.load(i).at()).leftNodeI(), -1));
		    b.setRightNode(nodes.value(tw.beam(tw.load(i).at()).rightNodeI(), -1));

		    Q_ASSERT(b.leftNodeI() != -1);
		    Q_ASSERT(b.rightNodeI() != -1);

		    writeBeam(b);
		    beams.insert(tw.load(i).at(), beamCount++);
                }
                ColinLoad l = tw.load(i);
                l.setBeam(beams.value(tw.load(i).at(), -1));
                Q_ASSERT(l.at() != -1);
				writeLoad(l, tw);

            }
            else
            {
                if(!nodes.contains(tw.load(i).at()))
                {
                    writeNode(tw.node(tw.load(i).at()));
                    nodes.insert(tw.load(i).at(), nodeCount++);
                }
                ColinLoad l = tw.load(i);
                l.setBeam(nodes.value(tw.load(i).at(), -1));
                Q_ASSERT(l.at() != -1);
				writeLoad(l, tw);

            }
        }
    }

    writeEndElement(); //struct
    writeEndDocument();

}

void XmlWriter::writeTw(const ColinStruct &tw, bool includeResults)
{
    setAutoFormatting(true);
    writeStartDocument();

    writeStartElement("ColinStruct");
	writeAttribute("version", "0.01");
    writeRequredLibEntries(tw);

    for(int i=0; i<tw.node_n(); i++)
		writeNode(tw.node(i), includeResults);

    for(int i=0; i<tw.beam_n(); i++)
		writeBeam(tw.beam(i), includeResults);

	for(int i=0; i<tw.bls_n(); i++)
		writeBLS(tw.bls(i));

	for(int i=0; i<tw.cls_n(); i++)
		writeCLS(tw.cls(i), tw);

    for(int i=0; i<tw.load_n(); i++)
		writeLoad(tw.load(i), tw);

    writeEndElement(); //struct
    writeEndDocument();

}

void XmlWriter::writeRequredLibEntries(const ColinStruct &tw, bool onlySelection)
{
    QList<int> writtenM;
    QList<int> writtenP;
    for(int i=0; i<tw.beam_n(); i++)
    {
        if(onlySelection && !tw.beam(i).isSelected())
            continue;
        if(!writtenM.contains(tw.beam(i).MatI()))
        {
            writeMat(tw.beam(i).Mat());
            writtenM.append(tw.beam(i).MatI());
        }
        if(!writtenP.contains(tw.beam(i).ProfileI()))
        {
            writeProfile(tw.beam(i).Profile());
            writtenP.append(tw.beam(i).ProfileI());
        }
    }
}

void XmlWriter::writeNode(const ColinNode &n, bool includeResults)
{
    writeStartElement("node");
    writeAttribute("X", QString("%1").arg(n.x()));
    writeAttribute("Z", QString("%1").arg(n.z()));
    if(n.hasbearing())
        writeBearing(n.bearing());
	if(n.getStruct()->isCalculated()){
		for(int i=0; i<n.getStruct()->cls_n(); i++){
			writeResult(n, i);
		}
	}
    writeEndElement();//node
}

void XmlWriter::writeBeam(const ColinBeam &s, bool includeResults)
{
    writeStartElement("beam");

    writeAttribute("from", QString("%1").arg(s.leftNodeI()));

    writeAttribute("to", QString("%1").arg(s.rightNodeI()));

    writeAttribute("material", QString("%1").arg(s.Mat().name()));
    writeAttribute("profile", QString("%1").arg(s.Profile().name()));

    if(s.hasJoints())
    {
        writeStartElement("joints");
        for(int i=0; i<6; i++)
            writeAttribute(QString("j%1").arg(i), QString("%1").arg(s.joint(i)));
        writeEndElement(); //joints

        writeStartElement("springs");
        for(int i=0; i<6; i++)
            writeAttribute(QString("s%1").arg(i), QString("%1").arg(s.spring(i)));
        writeEndElement(); //springs
    }

	if(s.getStruct()->isCalculated()){
		for(int i=0; i<s.getStruct()->cls_n(); i++){
			writeResult(s, i);
		}
	}

    writeEndElement();//beam;

}

void XmlWriter::writeLoad(const ColinLoad &l, const ColinStruct &tw)
{
    writeStartElement("load");
    writeAttribute("type", QString("%1").arg(l.typ()));
    writeAttribute("position", QString("%1").arg(l.at()));
    writeAttribute("Px", QString("%1").arg(l.Px()));
    writeAttribute("Pz", QString("%1").arg(l.Pz()));
    writeAttribute("M", QString("%1").arg(l.M()));
	if(l.set()>=0)
		writeAttribute("set", tw.bls(l.set()).name());
	else
		writeAttribute("set", "none");
    writeEndElement(); //load
}

void XmlWriter::writeBearing(const ColinSupport &b)
{
    writeStartElement("bearing");
    writeAttribute("form", QString("%1").arg(b.form()));

    writeAttribute("angle", QString("%1").arg(b.angle()));

    writeStartElement("springs");
    writeAttribute(QString("cX"), QString("%1").arg(b.c_x()));
    writeAttribute(QString("cZ"), QString("%1").arg(b.c_z()));
    writeAttribute(QString("cPhi"), QString("%1").arg(b.c_phi()));
    writeEndElement();//springs
    writeEndElement();//bearing
}

void XmlWriter::writeBLS(const ColinBLS &b)
{
	writeStartElement("bls");
	writeAttribute("name", b.name());
	writeEndElement();
}

void XmlWriter::writeCLS(const ColinCLS &c, const ColinStruct &tw)
{
	writeStartElement("cls");
	writeAttribute("name", c.name());
	for(int i=0; i<c.count(); i++)
	{
		writeStartElement("lschild");
		writeAttribute("blsName", tw.bls(c.bls(i)).name());
		writeAttribute("factor", QString("%1").arg(c.fac(i)));
		writeEndElement();
	}
	writeEndElement();
}

void XmlWriter::writeMat(const ColinMaterial &m)
{
    writeStartElement("material");
    writeAttribute("name", m.name());
    writeAttribute("E", QString("%1").arg(m.E()));
//    writeAttribute("v", QString("%1").arg(m.v()));
//    writeAttribute("roh", QString("%1").arg(m.roh()));
    writeAttribute("alphaT", QString("%1").arg(m.alphaT()));
    writeEndElement();//material
}

void XmlWriter::writeProfile(const ColinCrossSection &p)
{
    writeStartElement("profile");
    writeAttribute("name", p.name());
    writeAttribute("A", QString("%1").arg(p.A()));
    writeAttribute("I", QString("%1").arg(p.I()));
    writeAttribute("h", QString("%1").arg(p.h()));
//    writeAttribute("Aq", QString("%1").arg(p.Aq()));
    writeEndElement();//profile
}

void XmlWriter::writeResult(const ColinNode &n, const int &clsindex)
{
	const char format = 'f';
	const int precision = 10;

	writeStartElement("result");
	if(n.getStruct()->cls_n()>0)
		writeAttribute("cls", n.getStruct()->cls(clsindex).name());
	writeAttribute("u", QString::number(n.u(clsindex), format, precision));
	writeAttribute("w", QString::number(n.w(clsindex), format, precision));
	writeAttribute("phi", QString::number(n.phi(clsindex), format, precision));
	if(n.hasbearing())
	{
		writeAttribute("Ax", QString::number(n.Ax(clsindex), format, precision));
		writeAttribute("Az", QString::number(n.Az(clsindex), format, precision));
		writeAttribute("Am", QString::number(n.Am(clsindex), format, precision));
	}
	writeEndElement();//result
}

void XmlWriter::writeResult(const ColinBeam &s, const int &clsindex)
{
	const char format = 'f';
	const int precision = 10;
	const int prefix = 1;

	writeStartElement("result");
	if(s.getStruct()->cls_n()>0)
		writeAttribute("cls", s.getStruct()->cls(clsindex).name());
	writeAttribute("u", s.uconst(clsindex).toAscii(prefix, format, precision));
	writeAttribute("w", s.wconst(clsindex).toAscii(prefix, format, precision));
	writeAttribute("phi", s.phiconst(clsindex).toAscii(prefix, format, precision));
	writeAttribute("N", s.Nconst(clsindex).toAscii(prefix, format, precision));
	writeAttribute("Q", s.Qconst(clsindex).toAscii(prefix, format, precision));
	writeAttribute("M", s.Mconst(clsindex).toAscii(prefix, format, precision));
	writeEndElement();
}
