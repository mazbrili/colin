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

void XmlWriter::writeLoad(const wgv_tw &tw, const int &nr)
{
    const wgv_load &l = tw.load(nr);
    setAutoFormatting(true);
    writeStartDocument();
    writeStartElement("ColinClipboardLoad");
    writeAttribute("version", "0.01");
    writeAttribute("type", QString("%1").arg(l.typ()));
    writeAttribute("position", QString("%1").arg(l.at()));
    writeAttribute("Px", QString("%1").arg(l.Px()));
    writeAttribute("Pz", QString("%1").arg(l.Pz()));
    writeAttribute("M", QString("%1").arg(l.M()));
    writeEndElement();
    writeEndDocument();//ColinClipboardLoad



}

void XmlWriter::writeSelection(const wgv_tw &tw, QPointF translation)
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

            wgv_beam b = tw.beam(i);
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
            if(tw.load(i).typ() == wgv_load::decreasingLinearLoad    ||
               tw.load(i).typ() == wgv_load::increasingLinearLoad    ||
               tw.load(i).typ() == wgv_load::uniformlyDistibutedLoad ||
               tw.load(i).typ() == wgv_load::tempChange              ||
               tw.load(i).typ() == wgv_load::tempDiffrence           )
            {
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

		    wgv_beam b = tw.beam(tw.load(i).at());
		    b.setLeftNode(nodes.value(tw.beam(tw.load(i).at()).leftNodeI(), -1));
		    b.setRightNode(nodes.value(tw.beam(tw.load(i).at()).rightNodeI(), -1));

		    Q_ASSERT(b.leftNodeI() != -1);
		    Q_ASSERT(b.rightNodeI() != -1);

		    writeBeam(b);
		    beams.insert(tw.load(i).at(), beamCount++);
                }
                wgv_load l = tw.load(i);
                l.setBeam(beams.value(tw.load(i).at(), -1));
                Q_ASSERT(l.at() != -1);
                writeLoad(l);

            }
            else
            {
                if(!nodes.contains(tw.load(i).at()))
                {
                    writeNode(tw.node(tw.load(i).at()));
                    nodes.insert(tw.load(i).at(), nodeCount++);
                }
                wgv_load l = tw.load(i);
                l.setBeam(nodes.value(tw.load(i).at(), -1));
                Q_ASSERT(l.at() != -1);
                writeLoad(l);

            }
        }
    }

    writeEndElement(); //struct
    writeEndDocument();

}

void XmlWriter::writeTw(const wgv_tw &tw)
{
    setAutoFormatting(true);
    writeStartDocument();

    writeStartElement("ColinStruct");
    writeAttribute("version", "0.01");
    writeRequredLibEntries(tw);

    for(int i=0; i<tw.node_n(); i++)
        writeNode(tw.node(i));

    for(int i=0; i<tw.beam_n(); i++)
        writeBeam(tw.beam(i));

    for(int i=0; i<tw.load_n(); i++)
        writeLoad(tw.load(i));

    writeEndElement(); //struct
    writeEndDocument();

}

void XmlWriter::writeRequredLibEntries(const wgv_tw &tw, bool onlySelection)
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

void XmlWriter::writeNode(const wgv_node &n)
{
    writeStartElement("node");
    writeAttribute("X", QString("%1").arg(n.x()));
    writeAttribute("Z", QString("%1").arg(n.z()));
    if(n.hasbearing())
        writeBearing(n.bearing());
    writeEndElement();//node
}

void XmlWriter::writeBeam(const wgv_beam &s)
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

    writeEndElement();//beam;

}

void XmlWriter::writeLoad(const wgv_load &l)
{
    writeStartElement("load");
    writeAttribute("type", QString("%1").arg(l.typ()));
    writeAttribute("position", QString("%1").arg(l.at()));
    writeAttribute("Px", QString("%1").arg(l.Px()));
    writeAttribute("Pz", QString("%1").arg(l.Pz()));
    writeAttribute("M", QString("%1").arg(l.M()));
    writeEndElement(); //load
}

void XmlWriter::writeBearing(const wgv_bearing &b)
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


void XmlWriter::writeMat(const wgv_mat &m)
{
    writeStartElement("material");
    writeAttribute("name", m.name());
    writeAttribute("E", QString("%1").arg(m.E()));
//    writeAttribute("v", QString("%1").arg(m.v()));
//    writeAttribute("roh", QString("%1").arg(m.roh()));
    writeAttribute("alphaT", QString("%1").arg(m.alphaT()));
    writeEndElement();//material
}

void XmlWriter::writeProfile(const wgv_profile &p)
{
    writeStartElement("profile");
    writeAttribute("name", p.name());
    writeAttribute("A", QString("%1").arg(p.A()));
    writeAttribute("I", QString("%1").arg(p.I()));
    writeAttribute("h", QString("%1").arg(p.h()));
//    writeAttribute("Aq", QString("%1").arg(p.Aq()));
    writeEndElement();//profile
}
