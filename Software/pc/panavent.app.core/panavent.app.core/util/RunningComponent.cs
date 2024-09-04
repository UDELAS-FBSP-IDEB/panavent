using Microsoft.VisualBasic.CompilerServices;
using System;
using System.Runtime.CompilerServices;
using System.Threading;

namespace panavent.app.core.util
{ 
    public abstract class RunningComponent
        {
            protected bool vRunning;
            protected object vSyncRoot;
            protected Thread vRunningThread;
            private bool vStopOnNextCycle;

            public RunningComponent()
            {
                this.vSyncRoot = RuntimeHelpers.GetObjectValue(new object());
                this.vRunningThread = new Thread(new ThreadStart(this.ThreadCallback));
            }

            public bool Running
            {
                get
                {
                    object vSyncRoot = this.vSyncRoot;
                    ObjectFlowControl.CheckForSyncLockOnValueType(vSyncRoot);
                    bool lockTaken = false;
                    try
                    {
                        Monitor.Enter(vSyncRoot, ref lockTaken);
                        return this.vRunning;
                    }
                    finally
                    {
                        if (lockTaken)
                            Monitor.Exit(vSyncRoot);
                    }
                }
            }

            public bool Stopping
            {
                get
                {
                    object vSyncRoot = this.vSyncRoot;
                    ObjectFlowControl.CheckForSyncLockOnValueType(vSyncRoot);
                    bool lockTaken = false;
                    try
                    {
                        Monitor.Enter(vSyncRoot, ref lockTaken);
                        return this.vStopOnNextCycle;
                    }
                    finally
                    {
                        if (lockTaken)
                            Monitor.Exit(vSyncRoot);
                    }
                }
            }

            public virtual void Start()
            {
                object vSyncRoot = this.vSyncRoot;
                ObjectFlowControl.CheckForSyncLockOnValueType(vSyncRoot);
                bool lockTaken = false;
                try
                {
                    Monitor.Enter(vSyncRoot, ref lockTaken);
                    if (this.vRunning)
                        return;
                }
                finally
                {
                    if (lockTaken)
                        Monitor.Exit(vSyncRoot);
                }
                this.InitializeComponent();
                this.RunThread();
                this.vRunning = true;

        }

            public virtual void Stop() => this.Stop(false);

            public virtual void Stop(bool allowCompleteCycle)
            {
                object vSyncRoot = this.vSyncRoot;
                ObjectFlowControl.CheckForSyncLockOnValueType(vSyncRoot);
                bool lockTaken = false;
                try
                {
                    Monitor.Enter(vSyncRoot, ref lockTaken);
                    if (!this.vRunning)
                        return;
                    this.vStopOnNextCycle = true;
                    if (!allowCompleteCycle)
                    {
                        try
                        {
                            vRunningThread.Interrupt();
                            this.vRunningThread.Join();
                        }
                        catch (ThreadAbortException ex)
                        {
                            ProjectData.SetProjectError((Exception)ex);
                            ProjectData.ClearProjectError();
                        }
                        finally
                        {
                            this.vRunning = false;                           
                        }
                    }
                }
                finally
                {
                    if (lockTaken)
                        Monitor.Exit(vSyncRoot);
                }
            }

            protected abstract void RunNow();

            protected virtual void ThreadCallback()
            {
                object vSyncRoot1 = this.vSyncRoot;
                ObjectFlowControl.CheckForSyncLockOnValueType(vSyncRoot1);
                bool lockTaken1 = false;
                try
                {
                    Monitor.Enter(vSyncRoot1, ref lockTaken1);
                    this.vRunning = true;
                    this.vStopOnNextCycle = false;
                }
                finally
                {
                    if (lockTaken1)
                        Monitor.Exit(vSyncRoot1);
                }
                this.RunNow();
                object vSyncRoot2 = this.vSyncRoot;
                ObjectFlowControl.CheckForSyncLockOnValueType(vSyncRoot2);
                bool lockTaken2 = false;
                try
                {
                    Monitor.Enter(vSyncRoot2, ref lockTaken2);
                    this.vRunning = false;
                    this.vStopOnNextCycle = false;
                }
                finally
                {
                    if (lockTaken2)
                        Monitor.Exit(vSyncRoot2);
                }
            }

            protected virtual void InitializeComponent()
            {
            }

            protected virtual bool CanContinue()
            {
                object vSyncRoot = this.vSyncRoot;
                ObjectFlowControl.CheckForSyncLockOnValueType(vSyncRoot);
                bool lockTaken = false;
                try
                {
                    Monitor.Enter(vSyncRoot, ref lockTaken);
                    return !this.vStopOnNextCycle;
                }
                finally
                {
                    if (lockTaken)
                        Monitor.Exit(vSyncRoot);
                }
            }

            protected virtual void StopOnNextCycle()
            {
                object vSyncRoot = this.vSyncRoot;
                ObjectFlowControl.CheckForSyncLockOnValueType(vSyncRoot);
                bool lockTaken = false;
                try
                {
                    Monitor.Enter(vSyncRoot, ref lockTaken);
                    this.vStopOnNextCycle = true;
                }
                finally
                {
                    if (lockTaken)
                        Monitor.Exit(vSyncRoot);
                }
            }

            private void RunThread()
            { 
                this.vRunningThread.IsBackground = true;
                this.vRunningThread.Start();
            }
        }
    
}
