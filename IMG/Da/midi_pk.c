/*
** Return string name of 'key' and octave number
** (e.g., 'midi_pk(61)' is 'C#4', middle C sharp).
** Changed 1/1/89 (w/trepidation) to make midi_pk(60) = 'C3', psl
** See also: key2name(3)
*/
static char *p[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

extern char *sprintf();

char *
midi_pk(key)
char key;
{
    static char r[8];

    if (0 <= key && key <= 127)
	return(sprintf(r, "%s%d", p[key%12], key/12-2));
    return("???");
}
