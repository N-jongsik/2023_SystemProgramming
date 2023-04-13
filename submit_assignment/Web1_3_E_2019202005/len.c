#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    char buf[512];
    int len;

    if ((dir = opendir(".")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.') continue; // skip hidden files
            if (stat(ent->d_name, &st) == -1) continue; // skip files we can't stat

            len = sprintf(buf, "%s\t", ent->d_name); // print filename
            if (len < 8) printf("\t"); // pad with tabs if filename is short

            len = sprintf(buf, "%ld", st.st_size); // get file size
            if (st.st_size > 1000000000) len = sprintf(buf, "%.1fG", (float)st.st_size / 1000000000);
            else if (st.st_size > 1000000) len = sprintf(buf, "%.1fM", (float)st.st_size / 1000000);
            else if (st.st_size > 1000) len = sprintf(buf, "%.1fK", (float)st.st_size / 1000);

            printf("%s\n", buf); // print file size with appropriate units
        }
        closedir(dir);
    } else {
        perror("");
        return 1;
    }
    return 0;
}

