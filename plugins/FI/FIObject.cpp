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

#include "FIObject.h"
#include "FIDialog.h"
#include "Util.h"

#include <QtDebug>


FIObject::FIObject (QString profile, QString name)
{
  _profile = profile;
  _name = name;
  _plugin = QString("FI");
  _type = QString("indicator");
  _outputKey = QString("v");
  _hasOutput = TRUE;
  _inputObject = QString("symbol");
  _closeKey = QString("C");
  _volumeKey = QString("V");
  
  _bars = new Bars;
  
  _commandList << QString("update");
  _commandList << QString("dialog");
  _commandList << QString("output");
  _commandList << QString("load");
  _commandList << QString("save");
  _commandList << QString("output_keys");
}

FIObject::~FIObject ()
{
  delete _bars;
}

void
FIObject::clear ()
{
  _bars->clear();
}

int
FIObject::message (ObjectCommand *oc)
{
  int rc = 0;

  switch (_commandList.indexOf(oc->command()))
  {
    case 0:
      rc = update(oc);
      break;
    case 1:
      rc = dialog(oc);
      break;
    case 2:
      rc = output(oc);
      break;
    case 3:
      rc = load(oc);
      break;
    case 4:
      rc = save(oc);
      break;
    case 5:
      rc = outputKeys(oc);
      break;
    default:
      break;
  }
  
  return rc;
}

int
FIObject::update (ObjectCommand *oc)
{
  clear();
  
  // input object
  Object *io = (Object *) oc->getObject(_inputObject);
  if (! io)
  {
    qDebug() << "FIObject::update: invalid input" << _inputObject;
    return 0;
  }
  
  // get input bars
  ObjectCommand toc(QString("output"));
  if (! io->message(&toc))
  {
    qDebug() << "FIObject::update: message error" << io->plugin() << toc.command();
    return 0;
  }

  Bars *cbars = toc.getBars(_closeKey);
  if (! cbars)
  {
    qDebug() << "FIObject::update: invalid close bars" << _closeKey;
    return 0;
  }

  Bars *vbars = toc.getBars(_volumeKey);
  if (! vbars)
  {
    qDebug() << "FIObject::update: invalid volume bars" << _volumeKey;
    return 0;
  }

  QList<int> keys = cbars->_bars.keys();
  
  Bar *pcbar = 0;
  for (int pos = 0; pos < keys.size(); pos++)
  {
    Bar *cbar = cbars->value(keys.at(pos));
    if (! cbar)
      continue;

    Bar *vbar = vbars->value(keys.at(pos));
    if (! vbar)
      continue;
    
    if (pcbar)
    {
      double cdiff = cbar->v - pcbar->v;
      double force = vbar->v * cdiff;
      _bars->setValue(keys.at(pos), force);
    }

    pcbar = cbar;
  }
  
  return 1;
}

int
FIObject::dialog (ObjectCommand *oc)
{
  FIDialog *dialog = new FIDialog(oc->getObjects(), _name);
  dialog->setSettings(_inputObject, _closeKey, _volumeKey);
  connect(dialog, SIGNAL(signalDone(void *)), this, SLOT(dialogDone(void *)));
  dialog->setModified(FALSE);
  dialog->show();
  return 1;
}

void
FIObject::dialogDone (void *dialog)
{
  FIDialog *d = (FIDialog *) dialog;
  d->settings(_inputObject, _closeKey, _volumeKey);
  
  ObjectCommand oc(QString("modified"));
  emit signalMessage(oc);
}

int
FIObject::outputKeys (ObjectCommand *oc)
{
  QStringList keys;
  keys << _outputKey;
  oc->setValue(QString("output_keys"), keys);
  return 1;
}

int
FIObject::output (ObjectCommand *oc)
{
  outputKeys(oc);
  oc->setValue(_outputKey, _bars);
  return 1;
}

int
FIObject::load (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "FIObject::load: invalid QSettings";
    return 0;
  }
  
  _inputObject = settings->value(QString("input_object")).toString();
  _closeKey = settings->value(QString("close_key")).toString();
  _volumeKey = settings->value(QString("volume_key")).toString();

  return 1;
}

int
FIObject::save (ObjectCommand *oc)
{
  QSettings *settings = (QSettings *) oc->getObject(QString("QSettings"));
  if (! settings)
  {
    qDebug() << "FIObject::save: invalid QSettings";
    return 0;
  }
  
  settings->setValue(QString("plugin"), _plugin);
  settings->setValue(QString("input_object"), _inputObject);
  settings->setValue(QString("close_key"), _closeKey);
  settings->setValue(QString("volume_key"), _volumeKey);

  return 1;
}
