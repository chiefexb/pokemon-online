#ifndef ADVANCEDBATTLEDATA_H
#define ADVANCEDBATTLEDATA_H

#include "battledata.h"
#include "proxydatacontainer.h"

class AdvancedBattleData : public QObject, public BattleDataInherit<ProxyDataContainer, AdvancedBattleData>
{
public:
    typedef BattleDataInherit<ProxyDataContainer, AdvancedBattleData> baseClass;

    AdvancedBattleData(const BattleConfiguration *conf) : baseClass(conf) {
        timer.start(1000, this);
    }

    ~AdvancedBattleData() {

    }

    /* The code could be refactored to be included in battledata directly,
      with the regular battledata container having also the extras functions */
    void onSendOut(int spot, int previndex, ShallowBattlePoke* pokemon, bool)
    {
        int player = this->player(spot);
        int slot = this->slotNum(spot);

        if (slot != previndex) {
            team(player).switchPokemons(slot, previndex);

            fieldPoke(spot).setPoke(team(player).poke(slot));
        }

        team(player).setPoke(slot, pokemon, isPlayer(spot));

        fieldPoke(spot).onSendOut(pokemon);
    }

    void onKo(int spot)
    {
        tempPoke(spot).changeStatus(Pokemon::Koed);
        if (isPlayer(spot)) {
            poke(spot).changeStatus(Pokemon::Koed);
        }
    }

    void onHpChange(int spot, int newHp)
    {
        tempPoke(spot).setLife(newHp);
        if (isPlayer(spot)) {
            poke(spot).setLife(newHp);
        }
    }

    void onMajorStatusChange(int spot, int status, bool, bool)
    {
        //TODO: handle confusion better
        if (status != Pokemon::Confused) {
            tempPoke(spot).changeStatus(status);
            if (isPlayer(spot)) {
                poke(spot).changeStatus(status);
            }
        }
    }

    void onStatBoost(int spot, int stat, int boost, bool silent) {
        (void) silent;
        fieldPoke(spot).boostStat(stat, boost);
    }

    void onDynamicInfo(int spot, const BattleDynamicInfo &info) {
        for (int i = 0; i < 7; i++) {
            fieldPoke(spot).setBoost(i+1, info.boosts[i]);
        }
        field().zone(player(spot))->setHazards(info.flags);
    }

    void onDynamicStats(int spot, const BattleStats &stats) {
        for (int i = 0; i < 5; i++) {
            fieldPoke(spot).setStat(i+1, stats.stats[i]);
        }
    }

    void onStartWeather(int spot, int weather, bool ability) {
        (void) ability;
        (void) spot;
        field().setWeather(weather);
    }

    void onContinueWeather(int weather) {
        field().setWeather(weather);
    }

    void onEndWeather(int weather) {
        (void) weather;
        field().setWeather(Weather::NormalWeather);
    }

    void onClockStart(int player, int time) {
        team(player).setTimeLeft(time,true);
    }

    void onClockStop(int player, int time) {
        team(player).setTimeLeft(time,false);
    }

    bool areAdjacent (int poke1, int poke2) const {
        return abs(slotNum(poke1)-slotNum(poke2)) <= 1;
    }

    /* the temp poke code could be made generic */
    PokeProxy &tempPoke(int spot) {
        return *fieldPoke(spot).pokemon();
    }

    void timerEvent(QTimerEvent *) {
        if (team(Player1).ticking()) {
            team(Player1).setTimeLeft(team(Player1).time()-1, true);
        }
        if (team(Player2).ticking()) {
            team(Player2).setTimeLeft(team(Player2).time()-1, true);
        }
    }

private:
    QBasicTimer timer;
};

#endif // ADVANCEDBATTLEDATA_H
