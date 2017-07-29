//
// Created by morrigan on 28/07/17.
//

#ifndef DAIMAN_MGPPARSERTASK_H
#define DAIMAN_MGPPARSERTASK_H

struct MGPParserParams : public TaskParams{
    QString path;
};

struct MGPParserResult : TaskResult{
    MGPParserResult(std::shared_ptr<MGPParserParams> params) :
            TaskResult(params) {}

    std::shared_ptr<const MGPParserParams> get_params() const {
        return dynamic_pointer_cast<const MGPParserParams>(params);
    }

    void discard(std::string err_msg){
        TaskResult::discard(err_msg);
    }

    virtual void add_to_document(const BaseTask* task){
        TaskResult::add_to_document(task);
    };
};

class MGPParserTask : public BaseTask {
public:
    MGPParserTask(std::shared_ptr<MGPParserParams> params, node_desc_t idx_thread_pool);

    std::shared_ptr<const MGPParserResult> get_result() const {
        return dynamic_pointer_cast<const MGPParserResult>(result);
    }

protected:
    virtual void main_loop();

private:
    std::shared_ptr<QFile> file;
};


#endif //DAIMAN_MGPPARSERTASK_H
