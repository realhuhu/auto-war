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

QJsonObject mergeSetting(QJsonObject defaultSetting, const QJsonObject &rawSetting) {
    if (rawSetting.isEmpty()) return defaultSetting;

    QJsonObject mergedSetting;
    for (auto it = defaultSetting.begin(); it != defaultSetting.end(); ++it) {
        auto key = it.key();
        auto defaultItemObj = it.value().toObject();
        auto rawItemObj = rawSetting.value(key).toObject();

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

QJsonObject loadSetting(
        QJsonObject config,
        const QString &qqRemark,
        const QString &redRemark,
        QJsonObject defaultSetting
) {
    for (auto qqRef: config["account"].toArray()) {
        auto qqObj = qqRef.toObject();
        if (qqObj["remark"] != qqRemark) continue;

        for (auto redRef: qqObj["red"].toArray()) {
            auto redObj = redRef.toObject();
            if (redObj["remark"] != redRemark) continue;

            return redObj["setting"].toObject();
        }
    }

    return defaultSetting;
}

void sleep(std::atomic<bool> *stopFlag, float seconds) {
    if (seconds <= 0) return;

    auto sleepDuration = std::chrono::duration<float>(seconds);
    auto startTime = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - startTime < sleepDuration) {
        if (stopFlag->load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}