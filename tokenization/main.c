#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 1000   // 字典最大詞條數
#define MAX_WORD_LEN 20     // 單詞最大長度
#define MAX_SENT_LEN 1000    // 句子最大長度
#define MAX_PATH_DEPTH 1000  // 斷詞路徑最大深度

char dict[MAX_DICT_SIZE][MAX_WORD_LEN];
int dict_count = 0;

// 去除換行符號
void trim(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

/* * 遞迴函數：尋找所有可能的斷詞組合
 * input: 原始句子
 * offset: 目前處理到的字串索引位置
 * path: 目前累積的斷詞結果 (指標陣列)
 * depth: 目前路徑的單詞數量
 * fp_out: 輸出的檔案指標
 */
void find_segments(const char *input, int offset, char *path[], int depth, FILE *fp_out) {
    int input_len = strlen(input);

    // 基底情況 (Base Case)：如果已經處理到句子結尾
    if (offset == input_len) {
        for (int i = 0; i < depth; i++) {
            fprintf(fp_out, "%s", path[i]);
            // 單詞之間加上空白，最後一個單詞後不加
            if (i < depth - 1) {
                fprintf(fp_out, " ");
            }
        }
        fprintf(fp_out, "\n");
        return;
    }

    // 遞迴步驟：嘗試字典中的每一個詞
    // 檢查句子從目前 offset 開始，是否匹配字典中的某個詞
    for (int i = 0; i < dict_count; i++) {
        int word_len = strlen(dict[i]);
        
        // 如果剩餘長度足夠，且字串匹配
        if (offset + word_len <= input_len) {
            if (strncmp(input + offset, dict[i], word_len) == 0) {
                // 記錄這個詞到路徑中
                path[depth] = dict[i];
                
                // 往下繼續遞迴 (offset 前進，depth + 1)
                find_segments(input, offset + word_len, path, depth + 1, fp_out);
                
                // 回溯 (Backtrack) 發生在迴圈進入下一次迭代時，
                // path[depth] 會被新的詞覆蓋，或者迴圈結束返回上一層。
            }
        }
    }
}

int main() {
    FILE *f_dict = fopen("dictionary.txt", "r");
    if (!f_dict) {
        printf("Error: 無法開啟 dictionary.txt\n");
        return 1;
    }

    char line_buf[MAX_WORD_LEN];

    while(fgets(line_buf, sizeof(line_buf), f_dict)){
        trim(line_buf);
        strcpy(dict[dict_count], line_buf);
        dict_count++;
        if(dict_count >= MAX_DICT_SIZE) {
            printf("exceeding dictionary max size");
            break;
        }
    }

    fclose(f_dict);

    FILE *f_in = fopen("input.txt", "r");
    FILE *f_out = fopen("output.txt", "w");
    
    char sentence[MAX_SENT_LEN];
    char *path[MAX_PATH_DEPTH]; // 用來暫存遞迴過程中的單詞指標

    while (fgets(sentence, sizeof(sentence), f_in)) {
        trim(sentence);
        if (strlen(sentence) == 0) continue;

        // 呼叫遞迴函數開始斷詞
        find_segments(sentence, 0, path, 0, f_out);
        fprintf(f_out, "\n");
    }

    fclose(f_in);
    fclose(f_out);

    return 0;
}