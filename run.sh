#!/bin/bash
# PAR tool-chain by zbytopav & hajekm13 :-)
# https://gist.github.com/zbycz/7655258


if [[ $# -lt 5 ]] ; then
    echo Spusteni:
    echo "  ./run.sh typ_ziletky #proc|seq fronta binarka argumenty [infiniband]"
    echo
    echo '     napr: ./run.sh 4c 16 fast ./bin-par "data001.txt arg2 arg3"'
    echo '     napr: ./run.sh 4c seq short "time ./bin-seq" data/data004.txt'
    echo '     napr:
    for proc in 2 4 8 16 24 32; do
        for data in data001.txt data002.txt data003.txt; do
            for infini in true false; do
                    echo "./run.sh 4c $proc long ./bin-par data/$data $infini"
            done;
         done;
    done;'
    echo "Vice info: http://users.fit.cvut.cz/~soch/mi-par/run.html"
    exit 0
fi

ziletka=$1
proc=$2
fronta=$3
binarka=$4
data=$5
infiniband=$6

if [ -z "$6" ]; then
    infiniband=true;
fi

# add 1. 12. 2015
infiniband=false

jobfile="job@$ziletka@$proc@$fronta@$binarka@$data@$infiniband"
jobfile=${jobfile//[^a-z0-9A-Z@]/}
jobfile=${jobfile//@/-}

cat > $jobfile << EOT
#!/bin/sh

#  ===========================================================================
# |                                                                           |
# |             COMMAND FILE FOR SUBMITTING SGE JOBS                          |
# |                                                                           |
# |                                                                           |
# | SGE keyword statements begin with #$                                      |
# |                                                                           |
# | Comments begin with #                                                     |
# | Any line whose first non-blank character is a pound sign (#)              |
# | and is not a SGE keyword statement is regarded as a comment.              |
#  ===========================================================================

#  ===========================================================================
# |                                                                           |
# | Request Bourne shell as shell for job                                     |
# |                                                                           |
#  ===========================================================================
#$ -S /bin/sh

#  ===========================================================================
# |                                                                           |
# | Execute the job from the current working directory.                       |
# |                                                                           |
#  ===========================================================================
#$ -cwd

#  ===========================================================================
# |                                                                           |
# | Defines or redefines the path used for the standard error stream          |
# | of the job.                                                               |
# |                                                                           |
#  ===========================================================================
#$ -e .

#  ===========================================================================
# |                                                                           |
# | The path used for the standard output stream of the job.                  |
# |                                                                           |
#  ===========================================================================
#$ -o .

#  ===========================================================================
# |                                                                           |
# | Specifies that all environment variables active within the qsub utility   |
# | be exported to the context of the job.                                    |
# |                                                                           |
#  ===========================================================================
#$ -V

#  ===========================================================================
# |                                                                           |
# | Set network communications - over Ethernet or InfiniBand.                 |
# |   false - Network communications over Ethernet                            |
# |   true  - Network communications over Infniband                           |
# |                                                                           |
#  ===========================================================================
INFINIBAND="$infiniband"

#  ===========================================================================
# |                                                                           |
# | Parallel program with arguments.                                          |
# |                                                                           |
#  ===========================================================================
MY_PARALLEL_PROGRAM="$binarka $data"  #tady se meni parametry

#  ===========================================================================
# |                                                                           |
# | Export environment variable to execution nodes                            |
# |                                                                           |
#  ===========================================================================
# export MY_VARIABLE1="..."
# export MY_VARIABLE2="..."

echo "Parametry: $jobfile"
echo "Spoustim MPI s NSLOTS=\$NSLOTS";


# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#  ===========================================================================
# |                                                                           |
# | !!! Do not change !!!                                                     |
# | mpirun -np \$NSLOTS                                                        |
# | !!! Do not change !!!                                                     |
# |                                                                           |
#  ===========================================================================

if [[ \${INFINIBAND} = "true" ]]
then
 #  -------------------------------------------------------------------------
 # | Set network communication openMPI between nodes - InfiniBand (openib)   |
 #  -------------------------------------------------------------------------
 time mpirun -np \$NSLOTS \${MY_PARALLEL_PROGRAM}
else
  #  -------------------------------------------------------------------------
  # | Set network communication openMPI between nodes - Ethernet (tcp)        |
  #  -------------------------------------------------------------------------
  time mpirun --mca btl tcp,self -np \$NSLOTS \${MY_PARALLEL_PROGRAM}
fi
EOT


# -------------------------------------------------------------------------------------------


if [ "$proc" = "seq" ]; then
    proc=1
    cat > $jobfile <<EOT
# Request Bourne shell as shell for job
#$ -S /bin/sh
# Execute the job from the current working directory.
#$ -cwd
# Defines  or  redefines  the  path used for the standard error stream of the job.
#$ -e .
# The path used for the standard output stream of the job.
#$ -o .
# Do not change.
#$ -pe ompi 1
# Do not change.
#$ -q 12c_1slots_per_host.q
time $binarka $data
EOT
fi;


echo "Spoustim $(tput sgr 0 1)$jobfile$(tput sgr0)"
chmod +x $jobfile
#/opt/bin/qrun.sh $ziletka $proc $fronta $jobfile
qrun $ziletka $proc $fronta $jobfile

echo
echo "Moje ulohy:"
qstat | grep `whoami`
