#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "app_sql.h"

enum {
	SQL_FAILED = 0,
	SQL_SUCCESS
} sql_state_t;

sqlite3 *DCS003_db = NULL;
char* db_path = "DCS003.db";
//char* db_path = "dev_info.db";
pthread_mutex_t mutex_lock;

int app_sql_init()
{
	pthread_mutex_init(&mutex_lock, NULL);
	return SQL_SUCCESS;
}

int app_sql_open()
{
	//sqlite3 *DCS003_db;
	pthread_mutex_lock(&mutex_lock);
    
    if(sqlite3_open(db_path, &DCS003_db) != SQLITE_OK){//打开数据库
        printf("open sqlite db fail !!!\n");
        return SQL_FAILED;
    }

	return SQL_SUCCESS;
}

int app_sql_close()
{
    if ( sqlite3_close(DCS003_db) == SQLITE_BUSY ){
        return SQL_FAILED;
    }

    pthread_mutex_unlock(&mutex_lock);
	return SQL_SUCCESS;
}

#if 0
int app_sql_create()
{
	//sqlite3 * DCS003_db = *db;
	int ret = SQL_SUCCESS;
	char* zErrMsg = NULL;
	char *sql = NULL;

	ret = sqlite3_open_v2(db_path, &DCS003_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if( ret ){  
        printf( "Can't open database: %s\n", sqlite3_errmsg(DCS003_db));  
        ret = SQL_FAILED;
        goto Finish;
    }else{  
        printf( "Opened database successfully\n");  
    }

	if(sqlite3_exec(DCS003_db,"create table dal_cbt_clock("\
			"id integer PRIMARY KEY AUTO_INCREMENT,"\
			"groupid text,"\
			"startdate text,"\
			"state text,"\
			"percent text"\
			");",0,0,&zErrMsg) != SQLITE_OK){
		if(strcmp("table dal_cbt_clock already exists",zErrMsg) != 0){
			printf("zErrMsg = %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			ret = SQL_FAILED;
			goto Finish;
		}
	}

	if(sqlite3_exec(DCS003_db,//建表，存在则忽略    dal_cbt_controller
			"create table dal_cbt_controller("//表名
							"id integer PRIMARY KEY,"
							"box int,"
							"type int"
							");",0,0,&zErrMsg) != SQLITE_OK){
		if(strcmp("table dal_cbt_controller already exists",zErrMsg) != 0){
			printf("zErrMsg = %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			ret = SQL_FAILED;
			goto Finish;
		}
	}

	if(sqlite3_exec(DCS003_db,//建表，存在则忽略    dal_cbt_dp
			"create table dal_cbt_dp("//表名
							"id integer PRIMARY KEY,"
							"dp int,"
							"dpType int,"
							"dpNature int,"
							"dpSign int,"
							"port int,"
							"box int,"
							"channelNumber int,"
							"state int"
							");",0,0,&zErrMsg) != SQLITE_OK){
		if(strcmp("table dal_cbt_dp already exists",zErrMsg) != 0){
			printf("zErrMsg = %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			ret = SQL_FAILED;
			goto Finish;
		}
	}

	if(sqlite3_exec(DCS003_db,//建表，存在则忽略    dal_cbt_ecbox
			"create table dal_cbt_ecbox("//表名
							"id integer PRIMARY KEY,"
							"addr int,"
							"reSendTimes int,"
							"outtime int,"
							"heartBeatTime int,"
							"ip int,"
							"port int,"
							"bakip int,"
							"bakport int,"
							"vendorCode int,"
							"deviceNum int,"
							"softwareVersionNum int,"
							"softwareReleaseDate int,"
							"capacityInformationCode int,"
							"hardwareVersionNum int,"
							"hardwareReleaseDate int,"
							"remoteCommunicationVersionNum int,"
							"remoteCommunicationReleaseDate int,"
							"localCommunicationVersionNum int,"
							"localCommunicationReleaseDate int"
							");",0,0,&zErrMsg) != SQLITE_OK){
		if(strcmp("table dal_cbt_ecbox already exists",zErrMsg) != 0){
			printf("zErrMsg = %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			ret = SQL_FAILED;
			goto Finish;
		}
	}

	if(sqlite3_exec(DCS003_db,//建表，存在则忽略    dal_cbt_error
			"create table dal_cbt_error("//表名
							"id integer PRIMARY KEY,"
							"erc int,"
							"time int,"
							"state int"
							");",0,0,&zErrMsg) != SQLITE_OK){
		if(strcmp("table dal_cbt_error already exists",zErrMsg) != 0){
			printf("zErrMsg = %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			ret = SQL_FAILED;
			goto Finish;
		}
	}

	if(sqlite3_exec(DCS003_db,//建表，存在则忽略    dal_cbt_group
			"create table dal_cbt_group("//表名
							"id integer PRIMARY KEY,"
							"groupInfo int"
							");",0,0,&zErrMsg) != SQLITE_OK){
		if(strcmp("table dal_cbt_group already exists",zErrMsg) != 0){
			printf("zErrMsg = %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			ret = SQL_FAILED;
			goto Finish;
		}
	}

	if(sqlite3_exec(DCS003_db,//建表，存在则忽略    dal_cbt_simulate_dp
			"create table dal_cbt_simulate_dp("//表名
							"id integer PRIMARY KEY,"
							"time int,"
							"voltage int,"
							"current int,"
							"activePower int,"
							"reactivePower int,"
							"powerFactor int,"
							"opticalControl int,"
							"switchState int,"
							"apparentPower int,"
							"lampOnTime int,"
							"leakageCurrent int,"
							"activeEnergy int,"
							"positiveActiveEnergy int,"
							"reverseActiveEnergy int,"
							"reactiveEnergy int,"
							"positiveReactiveEnergy int,"
							"reverseReactiveEnergy int,"
							"apparentEnergy int"
							");",0,0,&zErrMsg) != SQLITE_OK){
		if(strcmp("table dal_cbt_simulate_dp already exists",zErrMsg) != 0){
			printf("zErrMsg = %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			ret = SQL_FAILED;
			goto Finish;
		}
	}

	if(sqlite3_exec(DCS003_db,//建表，存在则忽略    dal_cbt_simulate_ec
			"create table dal_cbt_simulate_ec("//表名
							"id integer PRIMARY KEY,"
							"time int,"
							"voltageA int,"
							"voltageB int,"
							"voltageC int,"
							"voltageALL int,"
							"currentA int,"
							"currentB int,"
							"currentC int,"
							"currentALL int,"
							"activePowerA int,"
							"activePowerB int,"
							"activePowerC int,"
							"activePowerALL int,"
							"reactivePowerA int,"
							"reactivePowerB int,"
							"reactivePowerC int,"
							"reactivePowerALL int,"
							"powerFactorA int,"
							"powerFactorB int,"
							"powerFactorC int,"
							"powerFactorALL int,"
							"opticalControl int,"
							"apparentPowerA int,"
							"apparentPowerB int,"
							"apparentPowerC int,"
							"apparentPowerALL int,"
							"leakageCurrent int,"
							"activeEnergyA int,"
							"activeEnergyB int,"
							"activeEnergyC int,"
							"activeEnergyALL int,"
							"positiveActiveEnergyA int,"
							"positiveActiveEnergyB int,"
							"positiveActiveEnergyC int,"
							"positiveActiveEnergyALL int,"
							"reverseActiveEnergyA int,"
							"reverseActiveEnergyB int,"
							"reverseActiveEnergyC int,"
							"reverseActiveEnergyALL int,"
							"reactiveEnergyA int,"
							"reactiveEnergyB int,"
							"reactiveEnergyC int,"
							"reactiveEnergyALL int,"
							"positiveReactiveEnergyA int,"
							"positiveReactiveEnergyB int,"
							"positiveReactiveEnergyC int,"
							"positiveReactiveEnergyALL int,"
							"reverseReactiveEnergyA int,"
							"reverseReactiveEnergyB int,"
							"reverseReactiveEnergyC int,"
							"reverseReactiveEnergyALL int,"
							"apparentEnergyA int,"
							"apparentEnergyB int,"
							"apparentEnergyC int,"
							"apparentEnergyALL int"
							");",0,0,&zErrMsg) != SQLITE_OK){
		if(strcmp("table dal_cbt_simulate_ec already exists",zErrMsg) != 0){
			printf("zErrMsg = %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
			ret = SQL_FAILED;
			goto Finish;
		}
	}

	Finish:
	app_sql_close();
	return ret;
}
#endif

#if 0
int main(void) {
	DCS003_db = NULL;
	app_sql_open();
	app_sql_create();
	app_sql_close();
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
#endif



