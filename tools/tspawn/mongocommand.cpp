/* Copyright (c) 2013-2019, AOYAMA Kazuharu
 * All rights reserved.
 *
 * This software may be used and distributed according to the terms of
 * the New BSD License, which is incorporated herein by reference.
 */

#include <QFile>
#include <QSettings>
#include <QVariant>
#include <TMongoDriver>
#include <TMongoCursor>
#include "mongocommand.h"

static QSettings *mongoSettings = nullptr;


MongoCommand::MongoCommand(const QString &path) :
    driver(new TMongoDriver),
    settingsPath(path)
{
    if (!mongoSettings) {
        if (!QFile::exists(settingsPath)) {
            qCritical("not found, %s", qPrintable(path));
        }
        mongoSettings = new QSettings(settingsPath, QSettings::IniFormat);
    }
}


MongoCommand::~MongoCommand()
{
    if (driver->isOpen()) {
        close();
    }
    delete driver;
}


bool MongoCommand::open(const QString &env)
{
    databaseName = mongoSettings->value(env + "/DatabaseName").toString().trimmed();
    printf("DatabaseName: %s\n", qPrintable(databaseName));

    QString host = mongoSettings->value(env +"/HostName").toString().trimmed();
    printf("HostName:     %s\n", qPrintable(host));

    int port = mongoSettings->value(env +"/Port").toInt();
    QString user = mongoSettings->value(env +"/UserName").toString().trimmed();
    QString pass = mongoSettings->value(env +"/Password").toString().trimmed();
    QString opts = mongoSettings->value(env +"/ConnectOptions").toString().trimmed();

    bool status = driver->open(databaseName, user, pass, host, port, opts);
    if (!status) {
        fprintf(stderr, "MongoDB open error\n");
    } else {
        printf("MongoDB opened successfully\n");
    }
    return status;
}


void MongoCommand::close()
{
    driver->close();
}


QStringList MongoCommand::getCollectionNames() const
{
    return (driver->isOpen()) ? driver->getCollectionNames() : QStringList();
}
