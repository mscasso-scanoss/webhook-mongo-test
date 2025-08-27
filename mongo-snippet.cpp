void CommonMongodProcessInterface::deleteRecordFromRecordStore(
    const boost::intrusive_ptr<ExpressionContext>& expCtx, RecordStore* rs, RecordId rID) const {
    assertIgnorePrepareConflictsBehavior(expCtx);
    writeConflictRetry(expCtx->opCtx, "MPI::deleteFromRecordStore", expCtx->ns.ns(), [&] {
        Lock::GlobalLock lk(expCtx->opCtx, MODE_IS);
        WriteUnitOfWork wuow(expCtx->opCtx);
        rs->deleteRecord(expCtx->opCtx, rID);
        wuow.commit();
    });
}

void CommonMongodProcessInterface::truncateRecordStore(
    const boost::intrusive_ptr<ExpressionContext>& expCtx, RecordStore* rs) const {
    assertIgnorePrepareConflictsBehavior(expCtx);
    writeConflictRetry(expCtx->opCtx, "MPI::truncateRecordStore", expCtx->ns.ns(), [&] {
        Lock::GlobalLock lk(expCtx->opCtx, MODE_IS);
        WriteUnitOfWork wuow(expCtx->opCtx);
        auto status = rs->truncate(expCtx->opCtx);
        tassert(5643000, "Unable to clear record store", status.isOK());
        wuow.commit();
    });
}

boost::optional<Document> CommonMongodProcessInterface::lookupSingleDocumentLocally(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    const NamespaceString& nss,
    const Document& documentKey) {
    AutoGetCollectionForReadMaybeLockFree autoColl(expCtx->opCtx, nss);
