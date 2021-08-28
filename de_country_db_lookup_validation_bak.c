#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


#define PREFIX_MATCH 1
#define NOT_PREFIX_MATCH 0
#define MAX_TREE_ROOT 2
#define NO_EDGE -1
#define LEFT_EDGE 0
#define RIGHT_EDGE 1


#define LOG_LEVEL_EMERGENCY   0
#define LOG_LEVEL_VERBOSE     1
#define LOG_LEVEL_ERR         2
#define LOG_LEVEL_INFO        3
#define LOG_LEVEL_DEBUG       4
#define LOG_LEVEL_DEBUG2      5

typedef enum _bool {
   false = 0,
   true = 1
}Bool;

FILE *debugFilefp = NULL;
Bool LogOn = false;
int log_level = 0;


#define LOG(level, fmt, args...)\
	do{\
		if( (LogOn == false) && (level == 0))\
			printf(fmt, ## args);\
		else if ((LogOn == true) && (level >= log_level ))\
			printf(fmt, ## args);\
	}while(0)							

#define MAX_GEO_LOCS            253

static const char GeoIP_country_code[MAX_GEO_LOCS][3] = {
    "--",
    "AP","EU","AD","AE","AF","AG","AI","AL","AM","AN",
    "AO","AQ","AR","AS","AT","AU","AW","AZ","BA","BB",
    "BD","BE","BF","BG","BH","BI","BJ","BM","BN","BO",
    "BR","BS","BT","BV","BW","BY","BZ","CA","CC","CD",
    "CF","CG","CH","CI","CK","CL","CM","CN","CO","CR",
    "CU","CV","CX","CY","CZ","DE","DJ","DK","DM","DO",
    "DZ","EC","EE","EG","EH","ER","ES","ET","FI","FJ",
    "FK","FM","FO","FR","FX","GA","GB","GD","GE","GF",
    "GH","GI","GL","GM","GN","GP","GQ","GR","GS","GT",
    "GU","GW","GY","HK","HM","HN","HR","HT","HU","ID",
    "IE","IL","IN","IO","IQ","IR","IS","IT","JM","JO",
    "JP","KE","KG","KH","KI","KM","KN","KP","KR","KW",
    "KY","KZ","LA","LB","LC","LI","LK","LR","LS","LT",
    "LU","LV","LY","MA","MC","MD","MG","MH","MK","ML",
    "MM","MN","MO","MP","MQ","MR","MS","MT","MU","MV",
    "MW","MX","MY","MZ","NA","NC","NE","NF","NG","NI",
    "NL","NO","NP","NR","NU","NZ","OM","PA","PE","PF",
    "PG","PH","PK","PL","PM","PN","PR","PS","PT","PW",
    "PY","QA","RE","RO","RU","RW","SA","SB","SC","SD",
    "SE","SG","SH","SI","SJ","SK","SL","SM","SN","SO",
    "SR","ST","SV","SY","SZ","TC","TD","TF","TG","TH",
    "TJ","TK","TM","TN","TO","TL","TR","TT","TV","TW",
    "TZ","UA","UG","UM","US","UY","UZ","VA","VC","VE",
    "VG","VI","VN","VU","WF","WS","YE","YT","RS","ZA",
    "ZM","ME","ZW","A1","A2","O1","AX","GG","IM","JE",
    "BL","MF"
};

static const char * GeoIP_country_name[MAX_GEO_LOCS] = {
    "N/A", "Asia/Pacific Region", "Europe", "Andorra",
    "United Arab Emirates", "Afghanistan", "Antigua and Barbuda",
    "Anguilla", "Albania", "Armenia", "Netherlands Antilles",
    "Angola", "Antarctica", "Argentina", "American Samoa", "Austria",
    "Australia", "Aruba", "Azerbaijan", "Bosnia and Herzegovina",
    "Barbados", "Bangladesh", "Belgium", "Burkina Faso", "Bulgaria",
    "Bahrain", "Burundi", "Benin", "Bermuda", "Brunei Darussalam",
    "Bolivia", "Brazil", "Bahamas", "Bhutan", "Bouvet Island",
    "Botswana", "Belarus", "Belize", "Canada",
    "Cocos (Keeling) Islands",
    "Congo, The Democratic Republic of the",
    "Central African Republic", "Congo", "Switzerland",
    "Cote D'Ivoire", "Cook Islands", "Chile", "Cameroon", "China",
    "Colombia", "Costa Rica", "Cuba", "Cape Verde",
    "Christmas Island", "Cyprus", "Czech Republic", "Germany",
    "Djibouti", "Denmark", "Dominica", "Dominican Republic",
    "Algeria", "Ecuador", "Estonia", "Egypt", "Western Sahara",
    "Eritrea", "Spain", "Ethiopia", "Finland","Fiji",
    "Falkland Islands (Malvinas)", "Micronesia, Federated States of",
    "Faroe Islands", "France", "France, Metropolitan", "Gabon",
    "United Kingdom", "Grenada", "Georgia", "French Guiana", "Ghana",
    "Gibraltar", "Greenland", "Gambia", "Guinea","Guadeloupe",
    "Equatorial Guinea", "Greece",
    "South Georgia and the South Sandwich Islands", "Guatemala",
    "Guam", "Guinea-Bissau", "Guyana", "Hong Kong",
    "Heard Island and McDonald Islands", "Honduras", "Croatia",
    "Haiti", "Hungary", "Indonesia", "Ireland", "Israel", "India",
    "British Indian Ocean Territory", "Iraq",
    "Iran, Islamic Republic of", "Iceland", "Italy", "Jamaica",
    "Jordan", "Japan", "Kenya", "Kyrgyzstan", "Cambodia", "Kiribati",
    "Comoros", "Saint Kitts and Nevis",
    "Korea, Democratic People's Republic of", "Korea, Republic of",
    "Kuwait", "Cayman Islands", "Kazakhstan",
    "Lao People's Democratic Republic", "Lebanon", "Saint Lucia",
    "Liechtenstein", "Sri Lanka", "Liberia", "Lesotho", "Lithuania",
    "Luxembourg", "Latvia", "Libyan Arab Jamahiriya", "Morocco",
    "Monaco", "Moldova, Republic of", "Madagascar",
    "Marshall Islands", "Macedonia", "Mali", "Myanmar", "Mongolia",
    "Macau", "Northern Mariana Islands", "Martinique", "Mauritania",
    "Montserrat", "Malta", "Mauritius", "Maldives", "Malawi",
    "Mexico", "Malaysia", "Mozambique", "Namibia", "New Caledonia",
    "Niger", "Norfolk Island", "Nigeria", "Nicaragua", "Netherlands",
 "Norway", "Nepal", "Nauru", "Niue", "New Zealand", "Oman",
    "Panama", "Peru", "French Polynesia", "Papua New Guinea",
    "Philippines", "Pakistan", "Poland", "Saint Pierre and Miquelon",
  "Pitcairn Islands", "Puerto Rico", "Palestinian Territory", "Portugal",
    "Palau", "Paraguay", "Qatar", "Reunion", "Romania",
    "Russian Federation", "Rwanda", "Saudi Arabia",
    "Solomon Islands", "Seychelles", "Sudan", "Sweden", "Singapore",
    "Saint Helena", "Slovenia", "Svalbard and Jan Mayen", "Slovakia",
    "Sierra Leone", "San Marino", "Senegal", "Somalia", "Suriname",
    "Sao Tome and Principe", "El Salvador", "Syrian Arab Republic",
    "Swaziland", "Turks and Caicos Islands", "Chad",
    "French Southern Territories", "Togo", "Thailand", "Tajikistan",
    "Tokelau", "Turkmenistan", "Tunisia", "Tonga", "Timor-Leste",
    "Turkey", "Trinidad and Tobago", "Tuvalu", "Taiwan",
    "Tanzania, United Republic of", "Ukraine", "Uganda",
    "United States Minor Outlying Islands", "United States",
    "Uruguay", "Uzbekistan", "Holy See (Vatican City State)",
    "Saint Vincent and the Grenadines", "Venezuela",
    "Virgin Islands, British", "Virgin Islands, U.S.", "Vietnam",
    "Vanuatu", "Wallis and Futuna", "Samoa", "Yemen", "Mayotte", "Serbia",
    "South Africa", "Zambia", "Montenegro", "Zimbabwe",
    "Anonymous Proxy", "Satellite Provider", "Other",
    "Aland Islands", "Guernsey", "Isle of Man","Jersey",
    "Saint Barthelemy", "Saint Martin"
};

short country_code_to_num(char *code)
{
        for(int i=1;i<MAX_GEO_LOCS;i++)
        {
                //printf("\n Country Code %s",GeoIP_country_code[i]);
                if(strncasecmp(GeoIP_country_code[i],code,3)== 0)
                                return i;
        }
        return -1;
}


void set_loglevel(int level)
{
	if ( (level >=LOG_LEVEL_EMERGENCY ) && (level <=LOG_LEVEL_DEBUG2 ))
	{
		log_level = level;
		LOG(LOG_LEVEL_EMERGENCY,"Log level set to level %d\n",level);
		return;
	}
	else
	{
		LOG(LOG_LEVEL_EMERGENCY,"\n !!! Invalid Log Level , Please enter between (0-4)!!! \n");
		return;
	}
}
void reset_loglevel()
{
	log_level = 0;
}
void logOn()
{
	LogOn = true;
}
void turnlogoff()
{
	LogOn = false;
}

void total_heap_memory_allocation();

unsigned int tree_node_cnt=0;
unsigned int linked_list_cnt=0;
unsigned int linked_list_node_cnt=0;

typedef struct _de_data_row_s
{
        unsigned int start_ip;
        unsigned int end_ip;
        //char three_letter_cn_name[4];
	short country_code;
        struct _de_data_row_s *next;
}__attribute__((packed)) de_data_row_t;

typedef struct _de_data_record_list_s
{
	unsigned int total_no_of_node;
	unsigned int prefix_len;
	de_data_row_t *head;
	de_data_row_t *tail;
}__attribute__((packed)) de_data_record_list_t;

typedef struct _de_tree_node_s
{
	struct _de_tree_node_s *left;
	struct _de_tree_node_s *right;
        // node_bit will have value either '0' or '1'
        unsigned int node_bit: 1;
        // node marker will indicate that a prefix match is available at this node
        unsigned int node_marker: 1;
        de_data_record_list_t *DR_List;
}__attribute__((packed)) de_tree_node_t;

//de_tree_node_t *root[MAX_TREE_ROOT];
de_tree_node_t  *root=NULL;

de_data_record_list_t *newDRList(void)
{
	de_data_record_list_t *newNode=(de_data_record_list_t*)malloc(siz