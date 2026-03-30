 #include "water_warning.h"
#include <stdio.h>
#include <string.h>
#include "array_list.h"
#include "data.h"

extern ArrayList globalRecordList;

// 品种阈值配置
typedef struct {
    // 正常范围
    float temp_min, temp_max;
    float oxy_min, oxy_max;
    float ph_min, ph_max;
    float ammonia_min, ammonia_max;

    // 一般警告
    float temp_a_min, temp_a_max;
    float oxy_a_min, oxy_a_max;
    float ph_a_min, ph_a_max;
    float ammonia_a_min, ammonia_a_max;

    char name[30];
} SpeciesConfig;

// 获取品种配置（严格按你给的配置文件）
static SpeciesConfig get_config(int mode) {
    SpeciesConfig cfg;
    memset(&cfg, 0, sizeof(SpeciesConfig)); // 初始化清零，避免脏数据

    if (mode == 1) { // 南美白对虾
        // 正常
        cfg.temp_min = 25; cfg.temp_max = 32;
        cfg.oxy_min = 5; cfg.oxy_max = 100;
        cfg.ph_min = 7.5; cfg.ph_max = 8.5;
        cfg.ammonia_min = 0; cfg.ammonia_max = 0.3;

        // 一般
        cfg.temp_a_min = 23; cfg.temp_a_max = 34;
        cfg.oxy_a_min = 3; cfg.oxy_a_max = 5;
        cfg.ph_a_min = 7.3; cfg.ph_a_max = 8.7;
        cfg.ammonia_a_min = 0.3; cfg.ammonia_a_max = 0.5;

        strcpy(cfg.name, "南美白对虾");
    }
    else if (mode == 2) { // 大口黑鲈
        // 正常
        cfg.temp_min = 20; cfg.temp_max = 30;
        cfg.oxy_min = 4; cfg.oxy_max = 100;
        cfg.ph_min = 7.4; cfg.ph_max = 8.6;
        cfg.ammonia_min = 0; cfg.ammonia_max = 0.2;

        // 一般
        cfg.temp_a_min = 18; cfg.temp_a_max = 32;
        cfg.oxy_a_min = 3; cfg.oxy_a_max = 4;
        cfg.ph_a_min = 7.2; cfg.ph_a_max = 8.8;
        cfg.ammonia_a_min = 0.2; cfg.ammonia_a_max = 0.4;

        strcpy(cfg.name, "大口黑鲈");
    }
    else if (mode == 3) { // 南澳牡蛎
        // 正常
        cfg.temp_min = 15; cfg.temp_max = 26;
        cfg.oxy_min = 5; cfg.oxy_max = 100;
        cfg.ph_min = 7.8; cfg.ph_max = 8.2;
        cfg.ammonia_min = 0; cfg.ammonia_max = 0.02;

        // 一般
        cfg.temp_a_min = 8; cfg.temp_a_max = 29;
        cfg.oxy_a_min = 4; cfg.oxy_a_max = 5;
        cfg.ph_a_min = 7.6; cfg.ph_a_max = 8.4;
        cfg.ammonia_a_min = 0.02; cfg.ammonia_a_max = 0.05;

        strcpy(cfg.name, "南澳牡蛎");
    }
    else {
        strcpy(cfg.name, "未知品种");
    }

    return cfg;
}

// 动态建议函数
static const char* get_temp_suggestion(float val, SpeciesConfig cfg) {
    if (val > cfg.temp_max) return "水温过高！请开启增氧+遮阳降温";
    if (val < cfg.temp_min) return "水温过低！请做好保温措施";
    return "水温正常，保持监测";
}

static const char* get_oxy_suggestion(float val, SpeciesConfig cfg) {
    if (val < cfg.oxy_min) return "溶氧量严重不足！立即全开增氧机";
    if (val > cfg.oxy_max) return "溶氧量过高，适当降低曝气";
    return "溶氧量正常，保持监测";
}

static const char* get_ph_suggestion(float val, SpeciesConfig cfg) {
    if (val > cfg.ph_max) return "pH偏高！少量投放降pH调节剂";
    if (val < cfg.ph_min) return "pH偏低！少量投放升pH调节剂";
    return "pH值正常，保持监测";
}

static const char* get_ammonia_suggestion(float val, SpeciesConfig cfg) {
    if (val > cfg.ammonia_max) return "氨氮超标！立即换水30%+底改+增氧";
    return "氨氮正常，保持监测";
}

void show_warning_after_login(int mode)
{
    if (globalRecordList == NULL || globalRecordList->size < 1) {
        printf("\n暂无水质数据！\n按回车继续...\n");
        getchar();
        return;
    }

    struct WaterQuality *latest = (struct WaterQuality*)getAList(globalRecordList, globalRecordList->size - 1);
    SpeciesConfig cfg = get_config(mode);

    int severe = 0, general = 0, normal = 0;

    // 统计1000条数据告警
    for (int i = 0; i < globalRecordList->size; i++) {
        struct WaterQuality *w = (struct WaterQuality*)getAList(globalRecordList, i);
        int lt=0, lo=0, lp=0, la=0;

        // 水温判断（按品种阈值）
        if (w->tmp < cfg.temp_a_min || w->tmp > cfg.temp_a_max) lt = 2;
        else if (w->tmp < cfg.temp_min || w->tmp > cfg.temp_max) lt = 1;

        // 溶氧判断
        if (w->doxygen < cfg.oxy_a_min) lo = 2;
        else if (w->doxygen < cfg.oxy_min) lo = 1;

        // pH判断
        if (w->ph < cfg.ph_a_min || w->ph > cfg.ph_a_max) lp = 2;
        else if (w->ph < cfg.ph_min || w->ph > cfg.ph_max) lp = 1;

        // 氨氮判断
        if (w->ammonia > cfg.ammonia_a_max) la = 2;
        else if (w->ammonia > cfg.ammonia_max) la = 1;

        if (lt==2 || lo==2 || lp==2 || la==2) severe++;
        else if (lt==1 || lo==1 || lp==1 || la==1) general++;
        else normal++;
    }

    // 界面展示（完全保留你现有样式）
    printf("\n");
    printf("==================== 水质智能预警 ====================\n");
    printf("✅ 当前养殖品种：%s\n", cfg.name);
    printf("📊 数据总量：%d 条\n", globalRecordList->size);
    printf("\n");

    printf("📌 正常生长范围\n");
    printf("   水温：%.1f ~ %.1f ℃\n", cfg.temp_min, cfg.temp_max);
    printf("   溶氧：%.1f ~ %.1f mg/L\n", cfg.oxy_min, cfg.oxy_max);
    printf("   PH值：%.1f ~ %.1f\n", cfg.ph_min, cfg.ph_max);
    printf("   氨氮：%.2f ~ %.2f mg/L\n", cfg.ammonia_min, cfg.ammonia_max);
    printf("\n");

    printf("⚠️ 一般警告范围\n");
    printf("   水温：%.1f ~ %.1f ℃\n", cfg.temp_a_min, cfg.temp_a_max);
    printf("   溶氧：%.1f ~ %.1f mg/L\n", cfg.oxy_a_min, cfg.oxy_a_max);
    printf("   PH值：%.1f ~ %.1f\n", cfg.ph_a_min, cfg.ph_a_max);
    printf("   氨氮：%.2f ~ %.2f mg/L\n", cfg.ammonia_a_min, cfg.ammonia_a_max);
    printf("\n");

    printf("🔥 异常统计\n");
    printf("   严重告警：%d 条\n", severe);
    printf("   一般告警：%d 条\n", general);
    printf("   正常数据：%d 条\n", normal);
    printf("\n");

    // 水温
    printf("💧 水温\n");
    printf("   最新值：%.1f ℃\n", latest->tmp);
    if (latest->tmp < cfg.temp_a_min || latest->tmp > cfg.temp_a_max)
        printf("   状态：❌ 严重告警\n");
    else if (latest->tmp < cfg.temp_min || latest->tmp > cfg.temp_max)
        printf("   状态：⚠️ 一般告警\n");
    else
        printf("   状态：✅ 正常\n");
    printf("   建议：%s\n", get_temp_suggestion(latest->tmp, cfg));
    printf("\n");

    // 溶氧量
    printf("💧 溶氧量\n");
    printf("   最新值：%.1f mg/L\n", latest->doxygen);
    if (latest->doxygen < cfg.oxy_a_min)
        printf("   状态：❌ 严重告警\n");
    else if (latest->doxygen < cfg.oxy_min)
        printf("   状态：⚠️ 一般告警\n");
    else
        printf("   状态：✅ 正常\n");
    printf("   建议：%s\n", get_oxy_suggestion(latest->doxygen, cfg));
    printf("\n");

    // pH
    printf("💧 pH值\n");
    printf("   最新值：%.1f\n", latest->ph);
    if (latest->ph < cfg.ph_a_min || latest->ph > cfg.ph_a_max)
        printf("   状态：❌ 严重告警\n");
    else if (latest->ph < cfg.ph_min || latest->ph > cfg.ph_max)
        printf("   状态：⚠️ 一般告警\n");
    else
        printf("   状态：✅ 正常\n");
    printf("   建议：%s\n", get_ph_suggestion(latest->ph, cfg));
    printf("\n");

    // 氨氮
    printf("💧 氨氮\n");
    printf("   最新值：%.2f mg/L\n", latest->ammonia);
    if (latest->ammonia > cfg.ammonia_a_max)
        printf("   状态：❌ 严重告警\n");
    else if (latest->ammonia > cfg.ammonia_max)
        printf("   状态：⚠️ 一般告警\n");
    else
        printf("   状态：✅ 正常\n");
    printf("   建议：%s\n", get_ammonia_suggestion(latest->ammonia, cfg));
    printf("\n");

    printf("======================================================\n");
    printf("按回车键继续...\n");
    getchar();
    getchar(); // 双getchar解决缓冲区问题
}