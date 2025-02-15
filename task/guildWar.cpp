# include "guildWar.h"


std::unique_ptr<Segment> locateMain() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/公会战役/前往.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>(
            "/公会战役/据点.png", Previous::NONE, false, Mode::RGB, 0, 0.98
    ));
    runUntil.emplace_back(std::make_unique<UntilImageStable>(
            "/公会战役/据点.png", Previous::NONE, false, Mode::RGB, 0, 0.98
    ));
    runUntil.emplace_back(std::make_unique<UntilImage>(
            "/公会战役/据点.png", Previous::NONE, false, Mode::RGB, 0, 0.98
    ));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    std::unique_ptr<Segment> res;
    auto candidateList = clicker->targetSegmentList;
    std::sort(candidateList.begin(), candidateList.end(), [](const Segment &a, const Segment &b) {
        return a.yCenter > b.yCenter;
    });
    for (const auto &candidate: candidateList) {
        clicker = std::make_unique<ImageClicker>("/公会战役/据点.png", candidate);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
                "/公会战役/当前驻守军队.png", "/公会战役/自动刷新.png"
        }));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        if (clicker->templatePath == "/公会战役/当前驻守军队.png") res = std::make_unique<Segment>(candidate);

        clicker = std::make_unique<ImageClicker>("/公会战役/关闭关卡窗口.png");

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/关闭关卡窗口.png", Previous::INNER, true));
        clicker->click(startUntil, clickUntil, runUntil);

        if (res) break;
    }

    return res;
}

std::unique_ptr<Segment> locateInfinite() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    RECT rect;
    GetWindowRect(state.hwnd, &rect);

    int left = rect.left;
    int right = rect.right;
    int top = rect.top;
    int bottom = rect.bottom;
    int xCenter = (right - left) / 2;
    int yCenter = (bottom - top) / 2;

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/公会战役/地图右下角.png");

        if (clicker->founded())break;

        Mouse::leftDown(state.hwnd, xCenter + 50, yCenter - 50);
        sleep(0.1);

        Mouse::moveTo(state.hwnd, xCenter - 150, yCenter - 250);
        sleep(0.1);

        Mouse::leftUp(state.hwnd, xCenter - 150, yCenter - 250);
        sleep(0.1);
    }

    while (!state.stopFlag.load()) {
        clicker = std::make_unique<ImageClicker>("/公会战役/据点关闭.png");

        if (clicker->founded())break;

        Mouse::leftDown(state.hwnd, xCenter + 50, yCenter - 50);
        sleep(0.1);

        Mouse::moveTo(state.hwnd, xCenter + 50, yCenter + 50);
        sleep(0.1);

        Mouse::leftUp(state.hwnd, xCenter + 50, yCenter + 50);
        sleep(0.1);
    }

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>(
            "/公会战役/据点.png", Previous::NONE, false, Mode::RGB, 0, 0.98
    ));
    clicker = clicker->locate(startUntil, runUntil);

    return std::make_unique<Segment>(similaritySelector(clicker->targetSegmentList));
}

void openAndToLastPage(std::unique_ptr<Segment> stronghold) {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/公会战役/据点.png", stronghold->copy());

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/最后一页.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
            "/公会战役/滚动条A.png", "/公会战役/滚动条B.png"
    }));
    clicker->click(startUntil, clickUntil, runUntil);
}

std::unique_ptr<Segment> locateEnemy() {
    auto intConfig = parseIntConfig("公会战役", "input", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    auto maxLevel = intConfig["最高打等级"];

    std::unique_ptr<Segment> s;

    while (!state.stopFlag.load()) {
        if (s) {
            Mouse::leftDown(state.hwnd, s->xCenter, s->yCenter);
            sleep(0.2);
            Mouse::leftUp(state.hwnd, s->xCenter, s->yCenter);
            sleep(0.2);
        }

        clicker = std::make_unique<ImageClicker>(std::vector<std::string>{
                "/公会战役/滚动条A.png", "/公会战役/滚动条B.png"
        });

        if (!s) s = std::make_unique<Segment>(similaritySelector(clicker->targetSegmentList));

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilCustom>(
                [&maxLevel](
                        std::unique_ptr<Segment>
                        &previous, Mode mode,
                        float threshold
                ) -> std::vector<Segment> {
                    auto ret = std::vector<Segment>{};

                    auto screen = CV::getScreen(mode);
                    auto levelSegmentList = CV::findPositions(
                            screen,
                            "/公会战役/等级.png",
                            threshold,
                            mode
                    );

                    auto attacksSegmentList = CV::findPositions(
                            screen,
                            "/公会战役/扫荡.png",
                            threshold,
                            mode
                    );

                    if (levelSegmentList.empty() || attacksSegmentList.empty()) return ret;

                    auto levelSegment = similaritySelector(levelSegmentList);

                    std::unique_ptr<Segment> currentTargetSegment = nullptr;
                    int currentTargetLevel = -1;

                    std::vector<Segment> sortedList = attacksSegmentList;

                    std::sort(sortedList.begin(), sortedList.end(), [](const Segment &a, const Segment &b) {
                        return a.yCenter < b.yCenter;
                    });

                    for (const auto &attackSegment: sortedList) {
                        auto x = levelSegment.x1;
                        auto y = attackSegment.y1;
                        auto w = levelSegment.x2 - levelSegment.x1;
                        auto h = attackSegment.y2 - attackSegment.y1;

                        cv::Mat targetBinary;
                        cv::threshold(screen(cv::Rect(x, y, w, h)),
                                      targetBinary, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

                        int windowWidth = 5;
                        int windowHeight = 8;
                        int resultLevel = 0;

                        for (int i = 0; i <= targetBinary.cols - windowWidth; ++i) {
                            for (int j = 0; j <= targetBinary.rows - windowHeight; ++j) {
                                cv::Mat croppedTargetBinary = targetBinary(
                                        cv::Rect(i, j, windowWidth, windowHeight));
                                if (!CV::hasBlack(croppedTargetBinary)) continue;

                                for (auto num: {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
                                    QFile numFile(QCoreApplication::applicationDirPath() +
                                                  QString("/res/公会战役/%1.png").arg(num));
                                    if (!numFile.open(QIODevice::ReadOnly))
                                        throw std::runtime_error("文件不存在: " + numFile.fileName().toStdString());
                                    QByteArray numByteArray = numFile.readAll();
                                    numFile.close();

                                    std::vector<char> data(numByteArray.data(),
                                                           numByteArray.data() + numByteArray.size());

                                    cv::Mat templateBinary;
                                    auto templateImg = cv::imdecode(cv::Mat(data), cv::COLOR_BGR2GRAY);
                                    cv::threshold(templateImg, templateBinary, 0, 255, cv::THRESH_BINARY);

                                    if (CV::identical(croppedTargetBinary, templateBinary)) {
                                        resultLevel *= 10;
                                        resultLevel += num;
                                        break;
                                    }
                                }
                            }
                        }

                        if (resultLevel == 0 || resultLevel > maxLevel || resultLevel < currentTargetLevel)
                            continue;

                        currentTargetLevel = resultLevel;
                        currentTargetSegment = std::make_unique<Segment>(attackSegment);
                    }

                    if (currentTargetSegment) ret.push_back(currentTargetSegment->copy());

                    return ret;
                }));
        clicker = clicker->drag(startUntil, clickUntil, similaritySelector, 1, 0, true, 50, false);

        if (clicker->founded()) return std::make_unique<Segment>(similaritySelector(clicker->targetSegmentList));

        if (std::make_unique<ImageClicker>("/公会战役/第一页.png")->founded()) return nullptr;

        clicker = std::make_unique<ImageClicker>("/公会战役/上一页.png");

        clearUntil(startUntil, clickUntil, runUntil);
        clicker->click(startUntil, clickUntil, runUntil, positionSelector("xCenter", "max"));
    }

    return nullptr;
}

bool attackEnemy(std::unique_ptr<Segment> enemy) {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/公会战役/扫荡.png", enemy->copy());

    clearUntil(startUntil, clickUntil, runUntil);
    startUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/可战斗.png"));
    runUntil.emplace_back(std::make_unique<UntilAnyImage>(list{
            "/公会战役/跳过战斗.png", "/公会战役/行动力不足.png"
    }));
    clicker = clicker->click(startUntil, clickUntil, runUntil);


    if (clicker->templatePath == "/公会战役/行动力不足.png") {
        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/确定.png"));
        clicker = clicker->locate(startUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/确定.png", Previous::INNER, true));
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/返回基地.png"));
        clicker->click(startUntil, clickUntil, runUntil);
        return true;
    }

    clearUntil(startUntil, clickUntil, runUntil);
    clickUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/结束战斗.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/一键领奖.png"));
    clicker->click(startUntil, clickUntil, runUntil);

    return false;
}

void backToBase() {
    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/公会战役/返回基地.png");

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/公会.png"));
    clicker->click(startUntil, clickUntil, runUntil);
}

void guildWar() {
    auto boolConfig = parseBoolConfig("公会战役", "checkbox", state.config);
    auto intConfig = parseIntConfig("公会战役", "input", state.config);
    auto stringConfig = parseStringConfig("公会战役", "select", state.config);

    std::unique_ptr<ImageClicker> clicker;

    std::vector<std::unique_ptr<Until>> startUntil;
    std::vector<std::unique_ptr<Until>> clickUntil;
    std::vector<std::unique_ptr<Until>> runUntil;

    clicker = std::make_unique<ImageClicker>("/公会战役/一键领奖.png");

    if (!clicker->founded()) {
        clicker = std::make_unique<ImageClicker>("/公会战役/公会.png");

        clearUntil(startUntil, clickUntil, runUntil);
        clickUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/公会争霸.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/公会战役.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/查看成员排名.png"));
        runUntil.emplace_back(
                std::make_unique<UntilIfImage>("/公会战役/进入战场.png", Previous::NONE, false, Mode::RGB, 0, 0.98));
        clicker = clicker->click(startUntil, clickUntil, runUntil);


        if (!clicker->founded()) {
            clicker = std::make_unique<ImageClicker>("/公会战役/关闭公会争霸窗口.png");

            clearUntil(startUntil, clickUntil, runUntil);
            runUntil.emplace_back(
                    std::make_unique<UntilImage>("/公会战役/关闭公会争霸窗口.png", Previous::INNER, true));
            clicker->click(startUntil, clickUntil, runUntil);
            return;
        }

        clearUntil(startUntil, clickUntil, runUntil);
        runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/一键领奖.png"));
        clicker = clicker->click(startUntil, clickUntil, runUntil);
    }

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/确定.png"));
    clicker = clicker->click(startUntil, clickUntil, runUntil);

    clearUntil(startUntil, clickUntil, runUntil);
    runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/确定.png", Previous::INNER, true));
    clicker->click(startUntil, clickUntil, runUntil);

    bool mainFinished = false;

    while (!state.stopFlag.load()) {
        if (boolConfig["领进度宝箱"]) {
            while (!state.stopFlag.load()) {
                clicker = std::make_unique<ImageClicker>("/公会战役/进度宝箱.png", 0, 0.95, 60, Mode::RGB);

                if (!clicker->founded()) break;

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>("/公会战役/领取奖励.png"));
                clicker = clicker->click(startUntil, clickUntil, runUntil);

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(
                        std::make_unique<UntilImage>("/公会战役/领取奖励.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil);
            }
        }

        emit Emitter::instance()->log(QString::fromStdString("战役模式: " + stringConfig["打战役模式"]));


        if (stringConfig["打战役模式"] == "模式1") {
            auto stronghold = locateMain();

            if (!stronghold) mainFinished = true;

            if (mainFinished) {
                backToBase();
                return;
            }

            openAndToLastPage(std::move(stronghold));

            auto enemy = locateEnemy();

            if (!enemy) {
                backToBase();
                return;
            }

            bool finished = attackEnemy(std::move(enemy));

            if (finished) {
                backToBase();
                return;
            }
        } else if (stringConfig["打战役模式"] == "模式2") {
            std::unique_ptr<Segment> stronghold;

            if (mainFinished) {
                stronghold = locateInfinite();
            } else {
                stronghold = locateMain();
            }

            if (!stronghold) {
                mainFinished = true;
                stronghold = locateInfinite();
            }

            openAndToLastPage(std::move(stronghold));

            auto enemy = locateEnemy();

            if (!enemy) {
                backToBase();
                return;
            }

            bool finished = attackEnemy(std::move(enemy));

            if (finished) {
                backToBase();
                return;
            }
        } else if (stringConfig["打战役模式"] == "模式3") {
            std::unique_ptr<Segment> stronghold;

            if (mainFinished) {
                stronghold = locateInfinite();
            } else {
                stronghold = locateMain();
            }

            if (!stronghold) {
                mainFinished = true;
                stronghold = locateInfinite();
            }

            openAndToLastPage(std::move(stronghold));

            auto enemy = locateEnemy();

            if (!enemy) {
                mainFinished = true;
                clicker = std::make_unique<ImageClicker>("/公会战役/关闭关卡窗口.png");

                clearUntil(startUntil, clickUntil, runUntil);
                runUntil.emplace_back(std::make_unique<UntilImage>(
                        "/公会战役/关闭关卡窗口.png", Previous::INNER, true));
                clicker->click(startUntil, clickUntil, runUntil);
                continue;
            }

            bool finished = attackEnemy(std::move(enemy));

            if (finished) {
                backToBase();
                return;
            }
        } else {
            auto stronghold = locateInfinite();

            openAndToLastPage(std::move(stronghold));

            auto enemy = locateEnemy();

            bool finished = attackEnemy(std::move(enemy));

            if (finished) {
                backToBase();
                return;
            }
        }
    }
}
