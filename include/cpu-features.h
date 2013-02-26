
struct cpu_features {
	unsigned long cpu_online;
};

typedef struct cpu_features cpu_features_t;

/* forward decl */
struct ps;

static inline const cpu_features_t *cpu_features(struct ps *ps)
{
	return ps->cpu_features;
}

//unsigned long cpu_f_no_cpu(cpu_features_t *);


/* core private */
void cpu_features_init(struct ps *ps);
