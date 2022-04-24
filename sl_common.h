#ifndef SL_COMMON_H
#define SL_COMMON_H

#define SL_REGISTER_COUNT 4

static fpos_t sl_find_line(FILE *f, int lineno)
{
    /* save file stream position before any manipulations with it */
    fpos_t last_pos;
    int rc = fgetpos(f, &last_pos);
    if (rc) {
        return rc;
    }

    if (lineno == 0) {
        return 0;
    }

    /* move pointer to first position */
    rc = fseek(f, 0, SEEK_SET);
    if (rc) {
        return rc;
    }

    fpos_t pos_count = 0;
    unsigned int line_count = 0;
    char *line = NULL;

    size_t len = 0;
    ssize_t read;

    /* read stream line by line with default delemiter '\n' and add string lengthes to counter
       to ensure line number refers to needed offset
    */
    while ((read = getline(&line, &len, f)) > 0) {

        line_count++;

        if(line_count > lineno) {
            break;
        }

        pos_count += strnlen(line, len);
    }

    /* revert stream position after manipulations */
    fseek(f, last_pos, SEEK_SET);

    if (line_count <= lineno) {
        pos_count = -1;
    }

    return pos_count;
}

#endif