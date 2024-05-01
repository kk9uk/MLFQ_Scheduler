## Input
***process.file*** contains the process information and its format is as follows:
> ProcessNum n  
> pid:?, arrival_time:?, execution_time:?  
> pid:?, arrival_time:?, execution_time:?  
> ...

## Configuration
***queue.cfg*** contains the queue information and its format is as follows:
> QueueNum n  
> Period_S ?  
> Time_Slice_Qn ? Allotmenttime_Qn ?  
> ...  
> Time_Slice_Q2 ? Allotmenttime_Q2 ?  
> Time_Slice_Q1 ? Allotmenttime_Q1 ?

## Output
***output.log*** will be generated as follows:
> Time_slot:?-?, pid:?, arrival-time:?, remaining_time:?  
> ...

## Usage
`$ make`  
`$ ./MLFQ-Scheduler`
