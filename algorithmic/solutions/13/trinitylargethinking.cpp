#include <stdio.h>
#include <stdlib.h>

int main() {
    int sx, sy;
    if (scanf("%d %d", &sx, &sy) != 2) return 0;
    int rx = sx, ry = sy;
    int curH = ry + 1;
    int curL = rx, curR = rx;
    int nxtL = -1, nxtR = -1;
    bool exploded = false;

    for (int turn = 1; turn <= 3000; ++turn) {
        int xm, ym;

        if (curH == 2 && nxtL != -1 && nxtL <= rx - 1 && nxtR >= rx + 1) {
            xm = 1; ym = 1; // any valid cell; explosion will happen after robot moves
        } else {
            bool curNeed = false;
            if (rx > 1 && curL > rx - 1) {
                curL--;
                xm = curL; ym = curH;
                curNeed = true;
            } else if (curR < rx + 1) {
                curR++;
                xm = curR; ym = curH;
                curNeed = true;
            }
            if (!curNeed) {
                if (nxtL == -1) {
                    nxtL = nxtR = rx;
                    xm = rx; ym = curH - 1;
                } else {
                    if (rx > 1 && nxtL > rx - 1) {
                        nxtL--;
                        xm = nxtL; ym = curH - 1;
                    } else if (nxtR < rx + 1) {
                        nxtR++;
                        xm = nxtR; ym = curH - 1;
                    } else {
                        if (curH > 2) {
                            curH--;
                            curL = nxtL;
                            curR = nxtR;
                            nxtL = nxtR = -1;
                            xm = rx; ym = curH - 1; // already black
                        } else {
                            xm = rx; ym = 1;
                        }
                    }
                }
            }
        }

        printf("%d %d\n", xm, ym);
        fflush(stdout);

        int nx, ny;
        if (scanf("%d %d", &nx, &ny) != 2) {
            break;
        }
        if (nx == 0 && ny == 0) {
            exploded = true;
            break;
        }
        rx = nx;
        ry = ny;
    }
    return 0;
}