#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

/**
 * Deterministik tek görevli simülasyon (FreeRTOS API kullanılmaz):
 * - Varış zamanına göre sırala.
 * - RT (0) kesintisiz, kullanıcı 1-3 kuantum=1 sn, kuantum sonunda öncelik düşür, en alt seviye RR.
 * - Her tick sadece tek olay işler, PDF'teki beklenen sırayı korur.
 */

static void log_start(Task* t, int tick)
{
    printf("%s%.4f sn\t\ttask%d basladi\t(id:%04d) Oncelik:%d kalan sure:%d sn%s\n",
           t->color, (double)tick, t->id, t->id, t->current_priority, t->remaining_time, COLOR_RESET);
}

static void log_run(Task* t, int tick)
{
    printf("%s%.4f sn\t\ttask%d yurutuluyor\t(id:%04d) Oncelik:%d kalan sure:%d sn%s\n",
           t->color, (double)tick, t->id, t->id, t->current_priority, t->remaining_time, COLOR_RESET);
}

static void log_suspend(Task* t, int tick)
{
    printf("%s%.4f sn\t\ttask%d askida\t\t(id:%04d) Oncelik:%d kalan sure:%d sn%s\n",
           t->color, (double)tick, t->id, t->id, t->current_priority, t->remaining_time, COLOR_RESET);
}

static void log_end(Task* t, int tick)
{
    printf("%s%.4f sn\t\ttask%d sonlandi\t(id:%04d) Oncelik:%d kalan sure:0 sn%s\n",
           t->color, (double)tick, t->id, t->id, t->current_priority, COLOR_RESET);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        fprintf(stderr, "Example: %s giris.txt\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];

    Task** tasks = NULL;
    int task_count = parse_input_file(input_file, &tasks);
    if (task_count <= 0) {
        fprintf(stderr, "Error: Task list could not be created or file is empty!\n");
        return 1;
    }

    // Varış zamanına göre sırala
    for (int i = 0; i < task_count - 1; i++) {
        for (int j = 0; j < task_count - i - 1; j++) {
            if (tasks[j]->arrival_time > tasks[j + 1]->arrival_time) {
                Task* tmp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = tmp;
            }
        }
    }

    // Kuyruklar
    Queue* rt_q = queue_create();
    Queue* user_q[3] = { queue_create(), queue_create(), queue_create() };

    int current_time = 0;
    int idx = 0;
    int completed = 0;
    const int max_time = 1000;

    while (current_time < max_time) {
        // Gelenleri ekle
        while (idx < task_count && tasks[idx]->arrival_time <= current_time) {
            Task* nt = tasks[idx];
            if (nt->priority == PRIORITY_RT) {
                queue_enqueue(rt_q, nt);
            } else {
                int qidx = nt->current_priority - 1;
                if (qidx < 0) qidx = 0;
                if (qidx > 2) qidx = 2;
                queue_enqueue(user_q[qidx], nt);
            }
            idx++;
        }

        Task* run = NULL;

        // RT varsa sadece RT çalıştır (kullanıcılar bekler)
        if (!queue_is_empty(rt_q)) {
            run = queue_dequeue(rt_q);
        } else {
            // RT yoksa kullanıcı: yüksekten düşüğe
            for (int k = 0; k < 3; k++) {
                if (!queue_is_empty(user_q[k])) {
                    run = queue_dequeue(user_q[k]);
                    break;
                }
            }
        }

        if (run == NULL) {
            current_time++;
            if (completed >= task_count && idx >= task_count) break;
            continue;
        }

        // Çalıştır (1 görev / 1 saniye)
        if (run->state != TASK_RUNNING) {
            run->state = TASK_RUNNING;
            log_start(run, current_time);
        }
        log_run(run, current_time);
        run->remaining_time--;

        if (run->remaining_time <= 0) {
            log_end(run, current_time + 1);
            completed++;
        } else {
            if (run->priority == PRIORITY_RT) {
                // RT bitene kadar RT kuyruğunda kalsın, askı logu yok
                queue_enqueue(rt_q, run);
            } else {
                log_suspend(run, current_time + 1);
                if (run->current_priority < PRIORITY_LOW) {
                    run->current_priority++;
                }
                int qidx = run->current_priority - 1;
                if (qidx < 0) qidx = 0;
                if (qidx > 2) qidx = 2;
                queue_enqueue(user_q[qidx], run);
            }
        }

        current_time++;
        if (completed >= task_count && idx >= task_count) break;
    }

    printf("\n=== Scheduler Simulation Completed ===\n");
    printf("Total time: %d seconds\n", current_time);
    printf("Completed tasks: %d/%d\n", completed, task_count);

    queue_destroy(rt_q);
    for (int k = 0; k < 3; k++) queue_destroy(user_q[k]);
    free_task_list(tasks, task_count);
    return 0;
}

