// UTF8toGesch.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void print_usage() {
    printf("Usage: UTF8toGesch <filename>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const char *in_filename = argv[1];
    char out_filename[1024];
    snprintf(out_filename, sizeof(out_filename), "%s.Gesch", in_filename);

    FILE *f_in = fopen(in_filename, "rb");
    if (!f_in) {
        perror("Error opening input file");
        return 1;
    }

    FILE *f_out = fopen(out_filename, "wb");
    if (!f_out) {
        perror("Error opening output file");
        fclose(f_in);
        return 1;
    }

    // Get file size
    fseek(f_in, 0, SEEK_END);
    uint64_t file_size = ftell(f_in);
    fseek(f_in, 0, SEEK_SET);

    if (file_size == 0) {
        printf("File is empty.\n");
        fclose(f_in);
        fclose(f_out);
        return 0;
    }

    printf("Converting %llu bytes...\n", (unsigned long long)file_size);

    uint8_t byte1, byte2, byte3;
    uint8_t new_byte;
    uint64_t bytes_processed = 0;
    int last_percent = -1;

    while (fread(&byte1, 1, 1, f_in) == 1) {
        bytes_processed++;

        // Progress display logic
        int current_percent = (int)((bytes_processed * 100) / file_size);
        if (current_percent > last_percent) {
            printf("\rDone %d%%", current_percent);
            fflush(stdout);
            last_percent = current_percent;
        }

        // --- UTF-8 Decoding Logic ---
        switch (byte1) {
            case 0xE2: {
                if (fread(&byte2, 1, 1, f_in) == 1 && fread(&byte3, 1, 1, f_in) == 1) {
                    bytes_processed += 2;
                    new_byte = 0; // default indicator, replace if matched
                    
                    if (byte2 == 0x80 && byte3 == 0x9E) new_byte = 246;
                    else if (byte2 == 0x80 && byte3 == 0x9F) new_byte = 247;
                    else if (byte2 == 0x80 && byte3 == 0x9C) new_byte = 244;
                    else if (byte2 == 0x80 && byte3 == 0x9D) new_byte = 245;
                    else if (byte2 == 0x80 && byte3 == 0x98) new_byte = 240;
                    else if (byte2 == 0x80 && byte3 == 0x99) new_byte = 241;
                    else if (byte2 == 0x80 && byte3 == 0x9A) new_byte = 242;
                    else if (byte2 == 0x80 && byte3 == 0x9B) new_byte = 243;
                    else if (byte2 == 0x80 && (byte3 == 0x93 || byte3 == 0x94)) new_byte = 196;
                    else if (byte2 == 0x80 && byte3 == 0xA6) new_byte = 255;
                    else if (byte2 == 0x80 && byte3 == 0x97) new_byte = 208;
                    else if (byte2 == 0x96 && byte3 == 0xAA) new_byte = 209;
                    else if (byte2 == 0x96 && byte3 == 0xAB) new_byte = 210;
                    else if (byte2 == 0x96 && byte3 == 0x80) new_byte = 28;
                    else if (byte2 == 0x96 && byte3 == 0x81) new_byte = 29;
                    else if (byte2 == 0x96 && byte3 == 0x84) new_byte = 30;
                    else if (byte2 == 0x8C && byte3 == 0xA0) new_byte = 209;
                    else if (byte2 == 0x8C && byte3 == 0xA1) new_byte = 210;
                    else if (byte2 == 0x89 && byte3 == 0x88) new_byte = 127;
                    else if (byte2 == 0x82 && byte3 == 0x8A) new_byte = 11;
                    else if (byte2 == 0x82 && byte3 == 0x8B) new_byte = 12;

                    if (new_byte != 0) {
                        fwrite(&new_byte, 1, 1, f_out);
                    } else {
                        // If no match was found, fall back (though BASIC logic didn't explicitly handle this edge case gracefully, we ignore or write raw)
                        fwrite(&byte1, 1, 1, f_out);
                        fseek(f_in, -2, SEEK_CUR); // Rewind
                        bytes_processed -= 2;
                    }
                } else {
                    fwrite(&byte1, 1, 1, f_out); // End of file edge case
                }
                break;
            }

            case 0xC2: {
                if (fread(&byte2, 1, 1, f_in) == 1) {
                    bytes_processed++;
                    new_byte = 0;
                    
                    if (byte2 == 0xA0) new_byte = 32;
                    else if (byte2 == 0xAB) new_byte = 252;
                    else if (byte2 == 0xBB) new_byte = 253;
                    else if (byte2 == 0xB5) new_byte = 249;
                    else if (byte2 == 0xA1) new_byte = 211;
                    else if (byte2 == 0xBF) new_byte = 212;

                    if (new_byte != 0) fwrite(&new_byte, 1, 1, f_out);
                    else {
                        fwrite(&byte1, 1, 1, f_out);
                        fseek(f_in, -1, SEEK_CUR);
                        bytes_processed--;
                    }
                } else {
                    fwrite(&byte1, 1, 1, f_out);
                }
                break;
            }

            case 0xC3: {
                if (fread(&byte2, 1, 1, f_in) == 1) {
                    bytes_processed++;
                    new_byte = 255; // Placeholder for unused
                    
                    if (byte2 == 0x84) new_byte = 0;
                    else if (byte2 == 0xA4) new_byte = 1;
                    else if (byte2 == 0x96) new_byte = 2;
                    else if (byte2 == 0xB6) new_byte = 3;
                    else if (byte2 == 0x9C) new_byte = 4;
                    else if (byte2 == 0xBC) new_byte = 5;
                    else if (byte2 == 0x9F) new_byte = 6;
                    else if (byte2 == 0x87) new_byte = 14;
                    else if (byte2 == 0xA7) new_byte = 15;
                    else if (byte2 == 0x82) new_byte = 198;
                    else if (byte2 == 0xA2) new_byte = 199;
                    else if (byte2 == 0x8A) new_byte = 200;
                    else if (byte2 == 0xAA) new_byte = 201;
                    else if (byte2 == 0x8E) new_byte = 202;
                    else if (byte2 == 0xAE) new_byte = 203;
                    else if (byte2 == 0x94) new_byte = 204;
                    else if (byte2 == 0xB4) new_byte = 205;
                    else if (byte2 == 0x9B) new_byte = 206;
                    else if (byte2 == 0xBB) new_byte = 207;
                    else if (byte2 == 0x89) new_byte = 16;
                    else if (byte2 == 0xA9) new_byte = 17;
                    else if (byte2 == 0x80) new_byte = 18;
                    else if (byte2 == 0xA0) new_byte = 19;
                    else if (byte2 == 0x88) new_byte = 20;
                    else if (byte2 == 0xA8) new_byte = 21;
                    else if (byte2 == 0x8C) new_byte = 22;
                    else if (byte2 == 0xAC) new_byte = 23;
                    else if (byte2 == 0x92) new_byte = 24;
                    else if (byte2 == 0xB2) new_byte = 25;
                    else if (byte2 == 0x99) new_byte = 26;
                    else if (byte2 == 0xB9) new_byte = 27;
                    else if (byte2 == 0xB7) new_byte = 31;
                    else if (byte2 == 0x8F) new_byte = 189;
                    else if (byte2 == 0xAF) new_byte = 190;
                    else if (byte2 == 0x91) new_byte = 213;
                    else if (byte2 == 0xB1) new_byte = 214;
                    else if (byte2 == 0x9D) new_byte = 215;
                    else if (byte2 == 0xBD) new_byte = 216;
                    else if (byte2 == 0x86) new_byte = 220;
                    else if (byte2 == 0xA6) new_byte = 221;
                    else if (byte2 == 0x81) new_byte = 7;
                    else if (byte2 == 0xA1) new_byte = 8;
                    else if (byte2 == 0x93) new_byte = 11;
                    else if (byte2 == 0xB3) new_byte = 12;
                    else if (byte2 == 0x8D) new_byte = 28;
                    else if (byte2 == 0xAD) new_byte = 29;
                    else if (byte2 == 0x9A) new_byte = 30;
                    else if (byte2 == 0xBA) new_byte = 31;

                    if (new_byte != 255) fwrite(&new_byte, 1, 1, f_out);
                    else {
                        fwrite(&byte1, 1, 1, f_out);
                        fseek(f_in, -1, SEEK_CUR);
                        bytes_processed--;
                    }
                } else {
                    fwrite(&byte1, 1, 1, f_out);
                }
                break;
            }

            case 0xC4: {
                if (fread(&byte2, 1, 1, f_in) == 1) {
                    bytes_processed++;
                    new_byte = 0;
                    
                    if (byte2 == 0x82) new_byte = 181;
                    else if (byte2 == 0x83) new_byte = 182;
                    else if (byte2 == 0x94) new_byte = 183;
                    else if (byte2 == 0x95) new_byte = 184;

                    if (new_byte != 0) fwrite(&new_byte, 1, 1, f_out);
                    else {
                        fwrite(&byte1, 1, 1, f_out);
                        fseek(f_in, -1, SEEK_CUR);
                        bytes_processed--;
                    }
                } else {
                    fwrite(&byte1, 1, 1, f_out);
                }
                break;
            }

            case 0xC5: {
                if (fread(&byte2, 1, 1, f_in) == 1) {
                    bytes_processed++;
                    new_byte = 0;
                    
                    if (byte2 == 0x92) new_byte = 222;
                    else if (byte2 == 0x93) new_byte = 223;
                    else if (byte2 == 0x8E) new_byte = 185;
                    else if (byte2 == 0x8F) new_byte = 186;

                    if (new_byte != 0) fwrite(&new_byte, 1, 1, f_out);
                    else {
                        fwrite(&byte1, 1, 1, f_out);
                        fseek(f_in, -1, SEEK_CUR);
                        bytes_processed--;
                    }
                } else {
                    fwrite(&byte1, 1, 1, f_out);
                }
                break;
            }

            case 0xD0: {
                if (fread(&byte2, 1, 1, f_in) == 1) {
                    bytes_processed++;
                    new_byte = 0;
                    
                    if (byte2 == 0x81) new_byte = 250;
                    else if (byte2 == 0x8E) new_byte = 187;
                    else if (byte2 >= 0x90 && byte2 <= 0xBF) {
                        new_byte = 128 + (byte2 - 0x90);
                    }

                    if (new_byte != 0) fwrite(&new_byte, 1, 1, f_out);
                    else {
                        fwrite(&byte1, 1, 1, f_out);
                        fseek(f_in, -1, SEEK_CUR);
                        bytes_processed--;
                    }
                } else {
                    fwrite(&byte1, 1, 1, f_out);
                }
                break;
            }

            case 0xD1: {
                if (fread(&byte2, 1, 1, f_in) == 1) {
                    bytes_processed++;
                    new_byte = 0;
                    
                    if (byte2 == 0x91) new_byte = 251;
                    else if (byte2 == 0x9E) new_byte = 188;
                    else if (byte2 >= 0x80 && byte2 <= 0x8F) {
                        new_byte = 224 + (byte2 - 0x80);
                    }

                    if (new_byte != 0) fwrite(&new_byte, 1, 1, f_out);
                    else {
                        fwrite(&byte1, 1, 1, f_out);
                        fseek(f_in, -1, SEEK_CUR);
                        bytes_processed--;
                    }
                } else {
                    fwrite(&byte1, 1, 1, f_out);
                }
                break;
            }

            default:
                fwrite(&byte1, 1, 1, f_out);
                break;
        }
    }

    printf("\nConversion complete.\n");

    fclose(f_in);
    fclose(f_out);

    return 0;
}

// Above is written by Gemini AI from:
/*
REM Converting UTF8 to Gesch codpage

$SCREENHIDE
$CONSOLE
_CONSOLE ON
_CONSOLETITLE "Gesch codepage convertor, revision 2"
_DEST _CONSOLE

REM Microsoft_windows_cp1251: 'Cyrillic alphabet such as Russian, Bulgarian, Serbian Cyrillic and other languages. It is the most widely used for encoding the Bulgarian, Serbian and Macedonian languages.
REM      128  129  130  131  132  133  134  135  136  137  138  139  140  141  142  143
REM DATA 1026,1027,8218,1107,8222,8230,8224,8225,8364,8240,1033,8249,1034,1036,1035,1039 '128+16*0 to 128+16*1-1
REM      144  145  146  147  148  149  150  151  152  153  154  155  156  157  158  159
REM DATA 1106,8216,8217,8220,8221,8226,8211,8212,0   ,8482,1113,8250,1114,1116,1115,1119 '128+16*1 to 128+16*2-1
REM      160  161  162  163  164  165  166  167  168  169  170  171  172  173  174  175
REM DATA 160 ,1038,1118,1032,164 ,1168,166 ,167 ,1025,169 ,1028,171 ,172 ,173 ,174 ,1031 '128+16*2 to 128+16*3-1
REM      176  177  178  179  180  181  182  183  184  185  186  187  188  189  190  191
REM DATA 176 ,177 ,1030,1110,1169,181 ,182 ,183 ,1105,8470,1108,187 ,1112,1029,1109,1111 '128+16*3 to 128+16*4-1
REM Cyrillic:
REM DATA 1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055 '128+16*4 to 128+16*5-1
REM DATA 1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071 '128+16*5 to 128+16*6-1
REM DATA 1072,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1086,1087 '128+16*6 to 128+16*7-1
REM DATA 1088,1089,1090,1091,1092,1093,1094,1095,1096,1097,1098,1099,1100,1101,1102,1103 '128+16*7 to 128+16*8-1 --\
REM                                                                                                                |
REM Microsoft_pc_cpGESCH: 'Gesch is Sanmayce's layout, combining the MIK and 437, in this way:                     |
REM                                                                                                                |
REM DATA 1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055 '128+16*0 to 128+16*1-1   |
REM DATA 1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071 '128+16*1 to 128+16*2-1   |
REM DATA 1072,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1086,1087 '128+16*2 to 128+16*3-1   |
REM                                                                                                                |
REM DATA 9617,9618,9619,9474,9508,9569,9570,9558,9557,9571,9553,9559,9565,9564,9563,9488 '128+16*3 to 128+16*4-1   |
REM DATA 9492,9524,9516,9500,9472,9532,9566,9567,9562,9556,9577,9574,9568,9552,9580,9575 '128+16*4 to 128+16*5-1   |
REM DATA 9576,9572,9573,9561,9560,9554,9555,9579,9578,9496,9484,9608,9604,9612,9616,9600 '128+16*5 to 128+16*6-1   |
REM                                                                                                                |
REM DATA 1088,1089,1090,1091,1092,1093,1094,1095,1096,1097,1098,1099,1100,1101,1102,1103 '128+16*6 to 128+16*7-1 <-/
REM      240  241  242  243  244  245  246  247  248  249  250  251  252  253  254  255
REM DATA 8216,8217,8218,8219,8220,8221,8222,8223,176 ,1118,1025,1105,171 ,187 ,175 ,8230 '128+16*7 to 128+16*8-1

'We need to replace 1251 only with the last line, above.

_DEFINE A-Z AS _INTEGER64

DIM byt AS STRING * 1
DIM byt2 AS STRING * 1
DIM byt3 AS STRING * 1
IF COMMAND$ = "" THEN PRINT "Usage: UTF8toGesch.exe filename": SYSTEM
OPEN COMMAND$ FOR BINARY AS #1
OPEN COMMAND$ + ".Gesch" FOR BINARY AS #2
qq = LOF(1)
'PRINT qq
'Grrr, below not the same as ? qq i.e. not as it should?!
'PRINT "Converting"; LOF(1); "bytes..."
PRINT "Converting"; qq; "bytes..."
LOCATE CSRLIN, 1: PRINT "Done"; (0 * 100) \ qq; "%";
FOR iii = 1 TO qq
    IF (iii * 100) \ qq > ((iii - 1) * 100) \ qq THEN LOCATE CSRLIN, 1: PRINT "Done"; (iii * 100) \ qq; "%";
    GET #1, iii, byt$
    SELECT CASE ASC(byt$)
        CASE &HE2:
            byt2$ = CHR$(32)
            byt3$ = CHR$(32)
            IF iii + 2 <= qq THEN
                GET #1, , byt2$
                GET #1, , byt3$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H9E THEN ' ,, lower " curved to left downwards
                iii = iii + 2
                bytNEW$ = CHR$(246)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H9F THEN ' `` upper " curved to right downwards
                iii = iii + 2
                bytNEW$ = CHR$(247)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H9C THEN ' upper " curved to right upwards
                iii = iii + 2
                bytNEW$ = CHR$(244)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H9D THEN ' upper " curved to left downwards
                iii = iii + 2
                bytNEW$ = CHR$(245)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H98 THEN ' upper ' curved to right upwards
                iii = iii + 2
                bytNEW$ = CHR$(240)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H99 THEN ' upper ' curved to left downwards
                iii = iii + 2
                bytNEW$ = CHR$(241)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H9A THEN ' lower ' curved to left downwards
                iii = iii + 2
                bytNEW$ = CHR$(242)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H9B THEN ' upper ' curved to right downwards
                iii = iii + 2
                bytNEW$ = CHR$(243)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND (ASC(byt3$) = &H93 OR ASC(byt3$) = &H94) THEN ' -
                iii = iii + 2
                bytNEW$ = CHR$(196)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &HA6 THEN ' ...
                iii = iii + 2
                bytNEW$ = CHR$(255)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 AND ASC(byt3$) = &H97 THEN ' double underscore
                iii = iii + 2
                bytNEW$ = CHR$(208)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H96 AND ASC(byt3$) = &HAA THEN ' filled box
                iii = iii + 2
                bytNEW$ = CHR$(209)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H96 AND ASC(byt3$) = &HAB THEN ' empty box
                iii = iii + 2
                bytNEW$ = CHR$(210)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H96 AND ASC(byt3$) = &H80 THEN ' upper box
                iii = iii + 2
                bytNEW$ = CHR$(28)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H96 AND ASC(byt3$) = &H81 THEN ' bold cursor
                iii = iii + 2
                bytNEW$ = CHR$(29)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H96 AND ASC(byt3$) = &H84 THEN ' lower box
                iii = iii + 2
                bytNEW$ = CHR$(30)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8C AND ASC(byt3$) = &HA0 THEN ' high integral
                iii = iii + 2
                bytNEW$ = CHR$(209)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8C AND ASC(byt3$) = &HA1 THEN ' low integral
                iii = iii + 2
                bytNEW$ = CHR$(210)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H89 AND ASC(byt3$) = &H88 THEN ' almost equal
                iii = iii + 2
                bytNEW$ = CHR$(127)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H82 AND ASC(byt3$) = &H8A THEN ' under +
                iii = iii + 2
                bytNEW$ = CHR$(11)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H82 AND ASC(byt3$) = &H8B THEN ' under -
                iii = iii + 2
                bytNEW$ = CHR$(12)
                PUT #2, , bytNEW$
            END IF

        CASE &HC2:
            byt2$ = CHR$(32)
            IF iii + 1 <= qq THEN
                GET #1, , byt2$
            END IF
            IF ASC(byt2$) = &HA0 THEN 'space
                iii = iii + 1
                bytNEW$ = CHR$(32)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HAB THEN '<<
                iii = iii + 1
                bytNEW$ = CHR$(252)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HBB THEN '>>
                iii = iii + 1
                bytNEW$ = CHR$(253)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HB5 THEN 'micro
                iii = iii + 1
                bytNEW$ = CHR$(249)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA1 THEN 'r!
                iii = iii + 1
                bytNEW$ = CHR$(211)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HBF THEN 'r?
                iii = iii + 1
                bytNEW$ = CHR$(212)
                PUT #2, , bytNEW$
            END IF


        CASE &HC3:
            byt2$ = CHR$(32)
            IF iii + 1 <= qq THEN
                GET #1, , byt2$
            END IF
            IF ASC(byt2$) = &H84 THEN 'A:
                iii = iii + 1
                bytNEW$ = CHR$(0)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA4 THEN 'a:
                iii = iii + 1
                bytNEW$ = CHR$(1)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H96 THEN 'O:
                iii = iii + 1
                bytNEW$ = CHR$(2)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HB6 THEN 'o:
                iii = iii + 1
                bytNEW$ = CHR$(3)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H9C THEN 'U:
                iii = iii + 1
                bytNEW$ = CHR$(4)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HBC THEN 'u:
                iii = iii + 1
                bytNEW$ = CHR$(5)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H9F THEN 'ss
                iii = iii + 1
                bytNEW$ = CHR$(6)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H87 THEN 'C,
                iii = iii + 1
                bytNEW$ = CHR$(14)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA7 THEN 'c,
                iii = iii + 1
                bytNEW$ = CHR$(15)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H82 THEN 'A^
                iii = iii + 1
                bytNEW$ = CHR$(198)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA2 THEN 'a^
                iii = iii + 1
                bytNEW$ = CHR$(199)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8A THEN 'E^
                iii = iii + 1
                bytNEW$ = CHR$(200)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HAA THEN 'e^
                iii = iii + 1
                bytNEW$ = CHR$(201)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8E THEN 'I^
                iii = iii + 1
                bytNEW$ = CHR$(202)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HAE THEN 'i^
                iii = iii + 1
                bytNEW$ = CHR$(203)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H94 THEN 'O^
                iii = iii + 1
                bytNEW$ = CHR$(204)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HB4 THEN 'o^
                iii = iii + 1
                bytNEW$ = CHR$(205)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H9B THEN 'U^
                iii = iii + 1
                bytNEW$ = CHR$(206)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HBB THEN 'u^
                iii = iii + 1
                bytNEW$ = CHR$(207)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H89 THEN 'E'
                iii = iii + 1
                bytNEW$ = CHR$(16)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA9 THEN 'e'
                iii = iii + 1
                bytNEW$ = CHR$(17)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H80 THEN 'A`
                iii = iii + 1
                bytNEW$ = CHR$(18)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA0 THEN 'a`
                iii = iii + 1
                bytNEW$ = CHR$(19)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H88 THEN 'E`
                iii = iii + 1
                bytNEW$ = CHR$(20)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA8 THEN 'e`
                iii = iii + 1
                bytNEW$ = CHR$(21)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8C THEN 'I`
                iii = iii + 1
                bytNEW$ = CHR$(22)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HAC THEN 'i`
                iii = iii + 1
                bytNEW$ = CHR$(23)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H92 THEN 'O`
                iii = iii + 1
                bytNEW$ = CHR$(24)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HB2 THEN 'o`
                iii = iii + 1
                bytNEW$ = CHR$(25)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H99 THEN 'U`
                iii = iii + 1
                bytNEW$ = CHR$(26)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HB9 THEN 'u`
                iii = iii + 1
                bytNEW$ = CHR$(27)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HB7 THEN 'division
                iii = iii + 1
                bytNEW$ = CHR$(31)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8F THEN 'I:
                iii = iii + 1
                bytNEW$ = CHR$(189)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HAF THEN 'i:
                iii = iii + 1
                bytNEW$ = CHR$(190)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H91 THEN 'N~
                iii = iii + 1
                bytNEW$ = CHR$(213)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HB1 THEN 'n~
                iii = iii + 1
                bytNEW$ = CHR$(214)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H9D THEN 'Y'
                iii = iii + 1
                bytNEW$ = CHR$(215)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HBD THEN 'y'
                iii = iii + 1
                bytNEW$ = CHR$(216)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H86 THEN 'AE
                iii = iii + 1
                bytNEW$ = CHR$(220)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA6 THEN 'ae
                iii = iii + 1
                bytNEW$ = CHR$(221)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H81 THEN 'A'
                iii = iii + 1
                bytNEW$ = CHR$(7)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HA1 THEN 'a'
                iii = iii + 1
                bytNEW$ = CHR$(8)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H93 THEN 'O'
                iii = iii + 1
                bytNEW$ = CHR$(11)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HB3 THEN 'o'
                iii = iii + 1
                bytNEW$ = CHR$(12)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8D THEN 'I'
                iii = iii + 1
                bytNEW$ = CHR$(28)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HAD THEN 'i'
                iii = iii + 1
                bytNEW$ = CHR$(29)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H9A THEN 'U'
                iii = iii + 1
                bytNEW$ = CHR$(30)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &HBA THEN 'u'
                iii = iii + 1
                bytNEW$ = CHR$(31)
                PUT #2, , bytNEW$
            END IF


        CASE &HC4:
            byt2$ = CHR$(32)
            IF iii + 1 <= qq THEN
                GET #1, , byt2$
            END IF
            IF ASC(byt2$) = &H82 THEN 'A kratko
                iii = iii + 1
                bytNEW$ = CHR$(181)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H83 THEN 'a kratko
                iii = iii + 1
                bytNEW$ = CHR$(182)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H94 THEN 'E kratko
                iii = iii + 1
                bytNEW$ = CHR$(183)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H95 THEN 'e kratko
                iii = iii + 1
                bytNEW$ = CHR$(184)
                PUT #2, , bytNEW$
            END IF


        CASE &HC5:
            byt2$ = CHR$(32)
            IF iii + 1 <= qq THEN
                GET #1, , byt2$
            END IF
            IF ASC(byt2$) = &H92 THEN 'OE
                iii = iii + 1
                bytNEW$ = CHR$(222)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H93 THEN 'oe
                iii = iii + 1
                bytNEW$ = CHR$(223)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8E THEN 'O kratko
                iii = iii + 1
                bytNEW$ = CHR$(185)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8F THEN 'o kratko
                iii = iii + 1
                bytNEW$ = CHR$(186)
                PUT #2, , bytNEW$
            END IF


        CASE &HD0:
            byt2$ = CHR$(32)
            IF iii + 1 <= qq THEN
                GET #1, , byt2$
            END IF
            IF ASC(byt2$) = &H81 THEN 'E:
                iii = iii + 1
                bytNEW$ = CHR$(250)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H8E THEN 'Y kratko
                iii = iii + 1
                bytNEW$ = CHR$(187)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) >= &H90 AND ASC(byt2$) <= &HBF THEN
                iii = iii + 1
                bytNEW$ = CHR$(128 + (ASC(byt2$) - &H90))
                PUT #2, , bytNEW$
            END IF

        CASE &HD1:
            byt2$ = CHR$(32)
            IF iii + 1 <= qq THEN
                GET #1, , byt2$
            END IF
            IF ASC(byt2$) = &H91 THEN 'e:
                iii = iii + 1
                bytNEW$ = CHR$(251)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) = &H9E THEN 'y(
                iii = iii + 1
                bytNEW$ = CHR$(188)
                PUT #2, , bytNEW$
            END IF
            IF ASC(byt2$) >= &H80 AND ASC(byt2$) <= &H8F THEN 'A..ya
                iii = iii + 1
                bytNEW$ = CHR$(224 + (ASC(byt2$) - &H80))
                PUT #2, , bytNEW$
            END IF

        CASE ELSE
            PUT #2, , byt$
    END SELECT


    'SELECT CASE ASC(byt$)
    '    CASE 130:
    '        bytNEW$ = CHR$(242)
    '        PUT #2, , bytNEW$

    '    CASE 132:
    '        bytNEW$ = CHR$(246)
    '        PUT #2, , bytNEW$

    '    CASE 133:
    '        bytNEW$ = CHR$(255)
    '        PUT #2, , bytNEW$

    '    CASE 145:
    '        bytNEW$ = CHR$(240)
    '        PUT #2, , bytNEW$

    '    CASE 146:
    '        bytNEW$ = CHR$(241)
    '        PUT #2, , bytNEW$

    '    CASE 147:
    '        bytNEW$ = CHR$(244)
    '        PUT #2, , bytNEW$

    '    CASE 148:
    '        bytNEW$ = CHR$(245)
    '        PUT #2, , bytNEW$

    '    CASE 152:
    '        bytNEW$ = CHR$(0)
    '        PUT #2, , bytNEW$

    '    CASE 160:
    '        bytNEW$ = CHR$(0)
    '        PUT #2, , bytNEW$

    '    CASE 162:
    '        bytNEW$ = CHR$(249)
    '        PUT #2, , bytNEW$

    '    CASE 168:
    '        bytNEW$ = CHR$(250)
    '        PUT #2, , bytNEW$

    '    CASE 184:
    '        bytNEW$ = CHR$(251)
    '        PUT #2, , bytNEW$

    '        'Not in the tables above, but should be changed [
    '    CASE 171:
    '        bytNEW$ = CHR$(252)
    '        PUT #2, , bytNEW$
    '    CASE 187:
    '        bytNEW$ = CHR$(253)
    '        PUT #2, , bytNEW$
    '    CASE 150:
    '        bytNEW$ = CHR$(196)
    '        PUT #2, , bytNEW$
    '    CASE 151:
    '        bytNEW$ = CHR$(196)
    '        PUT #2, , bytNEW$
    '    CASE 173:
    '        bytNEW$ = CHR$(196)
    '        PUT #2, , bytNEW$
    '        'Not in the tables above, but should be changed ]

    '    CASE 128 + 16 * 4 TO 128 + 16 * 5 - 1:
    '        bytNEW$ = CHR$(ASC(byt$) - (4 - 0) * 16)
    '        PUT #2, , bytNEW$
    '    CASE 128 + 16 * 5 TO 128 + 16 * 6 - 1:
    '        bytNEW$ = CHR$(ASC(byt$) - (5 - 1) * 16)
    '        PUT #2, , bytNEW$
    '    CASE 128 + 16 * 6 TO 128 + 16 * 7 - 1:
    '        bytNEW$ = CHR$(ASC(byt$) - (6 - 2) * 16)
    '        PUT #2, , bytNEW$
    '    CASE 128 + 16 * 7 TO 128 + 16 * 8 - 1:
    '        bytNEW$ = CHR$(ASC(byt$) - (7 - 6) * 16)
    '        PUT #2, , bytNEW$
    '    CASE ELSE:
    '        PUT #2, , byt$
    'END SELECT
NEXT
LOCATE CSRLIN, 1: PRINT "Done"; (qq * 100) \ qq; "%"
CLOSE #1, #2
SYSTEM


'Microsoft_pc_cpGESCH: 'Schpitz/Gesch (a.k.a. Georgievica a.k.a. Geschovica) is Sanmayce's layout, combining the MIK and 437, in this way: (NOT RECODING only 009,010,013):
'' shpitz

'' DEFINITIONS
'' top or extreme part
'' the ultimate, the best of, "cool"
'' typical, classic, a perfect example of; definitively

'' LANGUAGES OF ORIGIN
'' Yiddish

'' ETYMOLOGY
'' ????? shpits 'tip, peak'

'' ALTERNATIVE SPELLINGS
'' schpitz, shpits, shpitzy, shpitsy

''NOTES
''Steinmetz, "Yiddish and English" has an entry for shpits, but the meaning is simply 'tip.'

'' ASCII 000..031 have to accomodate German and French  '&H203e = 8254 is upperscore '&H00DC=220
'DATA 196,228,214,246,&H00DC,&H00FC,223,&H00c1,&H00e1,&h00C7,&H00E7
''A:a:O:o:U:u:ss A' a' C'c'
'DATA &H0c9,&H00e9,&H0c0,&H00e0,&H00c8,&H00e8,&H00cc,&H00ec,&H00d2,&H00f2,&H00d9,&H00f9
'' E'e' A`a`E`e`I`i`O`o`U`u`

''DATA &H00cb,&H00eb,&H00cf,&H00ef
'' E:e: I:i: ' E:e: are present in Russian, so remove them

''First half of big Cyrillic letters:
'DATA 1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055
''Second half of big Cyrillic letters:
'DATA 1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071
''First half of small Cyrillic letters:
'DATA 1072,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1086,1087
''1st third of CP437 drawing symbols:
'DATA 9617,9618,9619,9474,9508,9569,9570,9558,9557,9571,9553,9559,9565,9564,9563,9488
''2nd third of CP437 drawing symbols:
'DATA 9492,9524,9516,9500,9472,9532,9566,9567,9562,9556,9577,9574,9568,9552,9580,9575
''3rd third of CP437 drawing symbols:
'DATA 9576,9572,9573,9561,9560,9554,9555,9579,9578,9496,9484,9608,9604,9612,9616,9600
''Second half of small Cyrillic letters:
'DATA 1088,1089,1090,1091,1092,1093,1094,1095,1096,1097,1098,1099,1100,1101,1102,1103
''Last 16 of CP437 symbols:
''                                                 u-kr E:   e:   <<  >>  Up_ ...
''DATA 8216,8217,8218,8219,8220,8221,8222,8223,176,1118,1025,1105,171,187,175,8230
''                                                MICRO  E:   e:   <<  >>  Up_ ...
'DATA 8216,8217,8218,8219,8220,8221,8222,8223,176,&h00b5,1025,1105,171,187,175,8230

'DATA &H00c6,&H00E6,&H0152,&H0153
'' AE ae OEoe

'DATA &h2248
''almost equal to
''DATA &H221a
''square root

'DATA &H0102,&H0103,&H0114,&H0115,&H014e,&H014f,&H040e,&H045e,&H00cf,&H00ef
''Cyrillic short vowels: AaEeOoYy I:i:
'DATA &H00c2,&H00e2,&H00ca,&H00ea,&H00ce,&H00ee,&H00d4,&H00f4,&H00db,&H00fb
'' A^a^ E^e^ I^i^ O^o^ U^u^

'DATA &h2017,&H2320,&H2321,&h00a1,&h00bf,&h00d1,&h00f1,&h00dd,&h00fd
''doubleunderline IntegralH IntegralL, Spanish: r! r? N~ n~ Y` y`

'DATA &h00cd,&h00ed,&h00da,&h00fa
'' I'i'U'u'

'DATA &h00D3,&h00F3
'' O' o'

'' Portuguese makes use of five diacritics: the cedilla (c,), acute accent (a' e' i' o' u'), circumflex accent (a^, e^, o^), tilde (a~, o~), and grave accent (a`, and rarely e`, i`, o`, and u`).
*/
