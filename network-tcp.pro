#-------------------------------------------------
#
# Project created by QtCreator 2016-01-03T03:20:03
#
#-------------------------------------------------

QT += network

QT       += core websockets

QT       += core

QT       -= gui

QT += sql

TARGET = network-tcp

CONFIG   += console
CONFIG   += static
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES = \
    main.cpp \
    serverWeb.cpp


HEADERS = \
    serverWeb.h


