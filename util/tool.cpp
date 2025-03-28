#include "tool.h"

QJsonArray mergeCheckbox(QJsonArray defaultCheckboxArray, QJsonArray rawCheckboxArray) {
    QMap<QString, bool> rawCheckboxMap;

    for (int i = 0; i < rawCheckboxArray.count(); ++i) {
        auto rawCheckboxObj = rawCheckboxArray[i].toObject();
        rawCheckboxMap[rawCheckboxObj["text"].toString()] = rawCheckboxObj["value"].toBool();
    }

    for (int i = 0; i < defaultCheckboxArray.count(); ++i) {
        auto defaultCheckboxObj = defaultCheckboxArray[i].toObject();
        auto text = defaultCheckboxObj["text"].toString();

        if (!rawCheckboxMap.contains(text)) continue;

        defaultCheckboxObj["value"] = rawCheckboxMap[text];
        defaultCheckboxArray[i] = defaultCheckboxObj;
    }

    return defaultCheckboxArray;
}

QJsonArray mergeInput(QJsonArray defaultInputArray, QJsonArray rawInputArray) {
    QMap<QString, int> rawInputMap;

    for (int i = 0; i < rawInputArray.count(); ++i) {
        auto rawInputObj = rawInputArray[i].toObject();
        rawInputMap[rawInputObj["text"].toString()] = rawInputObj["value"].toInt();
    }

    for (int i = 0; i < defaultInputArray.count(); ++i) {
        auto defaultInputObj = defaultInputArray[i].toObject();
        auto text = defaultInputObj["text"].toString();

        if (!rawInputMap.contains(text)) continue;

        defaultInputObj["value"] = rawInputMap[text];
        defaultInputArray[i] = defaultInputObj;
    }

    return defaultInputArray;
}

QJsonArray mergeSelect(QJsonArray defaultSelectArray, QJsonArray rawSelectArray) {
    QMap<QString, QString> rawSelectMap;

    for (int i = 0; i < rawSelectArray.count(); ++i) {
        auto rawSelectObj = rawSelectArray[i].toObject();
        rawSelectMap[rawSelectObj["text"].toString()] = rawSelectObj["value"].toString();
    }

    for (int i = 0; i < defaultSelectArray.count(); ++i) {
        auto defaultSelectObj = defaultSelectArray[i].toObject();
        auto text = defaultSelectObj["text"].toString();
        auto candidateValue = rawSelectMap[text];

        if (!rawSelectMap.contains(text)) continue;
        if (!defaultSelectObj["options"].toArray().contains(candidateValue)) continue;

        defaultSelectObj["value"] = candidateValue;
        defaultSelectArray[i] = defaultSelectObj;
    }

    return defaultSelectArray;
}

QJsonObject merge(QJsonObject defaultConfig, const QJsonObject &rawConfig) {
    if (rawConfig.isEmpty()) return defaultConfig;

    QJsonObject mergedSetting;
    for (auto it = defaultConfig.begin(); it != defaultConfig.end(); ++it) {
        auto key = it.key();
        auto defaultItemObj = it.value().toObject();
        auto rawItemObj = rawConfig.value(key).toObject();

        if (rawItemObj.isEmpty()) {
            mergedSetting[key] = defaultItemObj;
            continue;
        }

        defaultItemObj["checkbox"] = mergeCheckbox(
                defaultItemObj["checkbox"].toArray(),
                rawItemObj["checkbox"].toArray()
        );

        defaultItemObj["input"] = mergeInput(
                defaultItemObj["input"].toArray(),
                rawItemObj["input"].toArray()
        );

        defaultItemObj["select"] = mergeSelect(
                defaultItemObj["select"].toArray(),
                rawItemObj["select"].toArray()
        );

        mergedSetting[key] = defaultItemObj;
    }

    return mergedSetting;
}
