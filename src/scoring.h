#ifndef SCORING_H

#define SCORING_H

#include <glib.h>
#include "matcher.h"
#include "procmsg.h"

#define MAX_SCORE 9999
#define MIN_SCORE -9999

struct _ScoringProp {
	MatcherList * matchers;
	int score;
};

typedef struct _ScoringProp ScoringProp;

extern GSList * prefs_scoring;


ScoringProp * scoringprop_new(MatcherList * matchers, int score);
void scoringprop_free(ScoringProp * prop);
gint scoringprop_score_message(ScoringProp * scoring, MsgInfo * info);

ScoringProp * scoringprop_parse(gchar ** str);


gint score_message(GSList * scoring_list, MsgInfo * info);

void prefs_scoring_write_config(void);
void prefs_scoring_read_config(void);
gchar * scoringprop_to_string(ScoringProp * prop);

#endif
