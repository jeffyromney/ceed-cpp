#ifndef PROPERTYUDIM_H
#define PROPERTYUDIM_H

#include "src/cegui/CEGUIUtils.h"
#include "QtnProperty/Core/PropertyFloat.h"
#include "QtnProperty/Auxiliary/PropertyTemplates.h"
#include "QtnProperty/Delegates/Utils/PropertyDelegateMisc.h"
#include "QtnProperty/StructPropertyBase.h"
#include <CEGUI/UVector.h>

class QtnPropertyUDimBase: public QtnStructPropertyBase<CEGUI::UDim, QtnPropertyFloatCallback>
{
    Q_OBJECT
    QtnPropertyUDimBase(const QtnPropertyUDimBase& other) Q_DECL_EQ_DELETE;

public:
    explicit QtnPropertyUDimBase(QObject* parent);

    QtnProperty* createScaleProperty();
    QtnProperty* createOffsetProperty();

protected:
    bool fromStrImpl(const QString& str, QtnPropertyChangeReason reason) override;
    bool toStrImpl(QString& str) const override;

    P_PROPERTY_DECL_MEMBER_OPERATORS(QtnPropertyUDimBase)
};

P_PROPERTY_DECL_EQ_OPERATORS(QtnPropertyUDimBase, CEGUI::UDim)

class QtnPropertyUDimCallback: public QtnSinglePropertyCallback<QtnPropertyUDimBase>
{
    Q_OBJECT
    QtnPropertyUDimCallback(const QtnPropertyUDimCallback& other) Q_DECL_EQ_DELETE;

public:
    explicit QtnPropertyUDimCallback(QObject* parent)
        : QtnSinglePropertyCallback<QtnPropertyUDimBase>(parent)
    {
    }

    P_PROPERTY_DECL_MEMBER_OPERATORS2(QtnPropertyUDimCallback, QtnPropertyUDimBase)
};

class QtnPropertyUDim: public QtnSinglePropertyValue<QtnPropertyUDimBase>
{
    Q_OBJECT
    QtnPropertyUDim(const QtnPropertyUDim& other) Q_DECL_EQ_DELETE;

public:
    explicit QtnPropertyUDim(QObject* parent)
        : QtnSinglePropertyValue<QtnPropertyUDimBase>(parent)
    {
    }

    P_PROPERTY_DECL_MEMBER_OPERATORS2(QtnPropertyUDim, QtnPropertyUDimBase)
};

class QtnPropertyDelegateUDim: public QtnPropertyDelegateTypedEx<QtnPropertyUDimBase>
{
    Q_DISABLE_COPY(QtnPropertyDelegateUDim)

public:
    QtnPropertyDelegateUDim(QtnPropertyUDimBase& owner);

    static void Register(QtnPropertyDelegateFactory& factory);

protected:
    QWidget* createValueEditorImpl(QWidget* parent, const QRect& rect, QtnInplaceInfo* inplaceInfo = nullptr) override;
    bool propertyValueToStrImpl(QString& strValue) const override;
};

Q_DECLARE_METATYPE(CEGUI::UDim);

#endif // PROPERTYUDIM_H
