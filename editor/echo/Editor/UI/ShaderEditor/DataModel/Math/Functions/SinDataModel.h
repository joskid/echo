#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>
#include <nodeeditor/NodeDataModel>
#include <iostream>
#include "ShaderDataModel.h"
#include "DataFloat.h"
#include "DataAny.h"
#include "ShaderData.h"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

namespace DataFlowProgramming
{
    class SinDataModel : public ShaderDataModel
    {
        Q_OBJECT

    public:
        SinDataModel();
        virtual ~SinDataModel() {}

        // caption
        QString caption() const override { return QStringLiteral("Sin"); }

        // is caption visible
        bool captionVisible() const override { return true; }

        // name
        QString name() const override { return QStringLiteral("Sin"); }

		// generate code
		virtual bool generateCode(ShaderCompiler& compiler) override;

    public:
        // load|save
        virtual QJsonObject save() const override;
        virtual void restore(QJsonObject const &p) override;

    public:
        // when input changed
        void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

        // widget
        QWidget* embeddedWidget() override { return nullptr; }
    };
}
