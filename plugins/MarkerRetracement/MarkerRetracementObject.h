/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#ifndef PLUGIN_MARKER_BUY_RETRACEMENT_HPP
#define PLUGIN_MARKER_BUY_RETRACEMENT_HPP

#include <QStringList>
#include <QColor>
#include <QPainter>
#include <QList>
#include <QRegion>
#include <QPoint>
#include <QDateTime>

#include "Object.h"


class MarkerRetracementObject : public Object
{
  Q_OBJECT
  
  public:
    enum Status
    {
      _NONE,
      _SELECTED,
      _CREATE,
      _MOVING,
      _MOVING_2
    };

    MarkerRetracementObject (QString profile, QString name);
    ~MarkerRetracementObject ();
    void clear ();
    int message (ObjectCommand *);
    int draw (ObjectCommand *);
    int info (ObjectCommand *);
    int highLowRange (ObjectCommand *);
    int mouseMove (ObjectCommand *);
    int mousePress (ObjectCommand *);
    int create (ObjectCommand *);
    int icon (ObjectCommand *);
    int load (ObjectCommand *);
    int save (ObjectCommand *);
    int update (ObjectCommand *);
    int copy (ObjectCommand *);
    int settings (ObjectCommand *);
    int dialog (ObjectCommand *);
    
    int load ();
    int save ();
    int isSelected (QPoint);
    int isGrabSelected (QPoint);
    void removeLocal ();
    void sendSelectedMessage ();
    void sendUnselectedMessage ();
    void sendDrawMessage ();
    void sendDeleteMessage ();
    void sendMovingMessage ();
    
  public slots:
    void dialogDone (void *);
    
  private:
    QStringList _commandList;
    int _penWidth;
    QColor _color;
    bool _selected;
    bool _modified;
    int _handleWidth;
    bool _readOnly;
    QList<QRegion> _selectionArea;
    QList<QRegion> _grabHandles;
    int _status;
    QDateTime _startDate;
    QDateTime _endDate;
    double _highPrice;
    double _lowPrice;
    bool _extend;
    int _index;
    int _index2;
    double _level1;
    double _level2;
    double _level3;
    QString _plotObject;
};

#endif
